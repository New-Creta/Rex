#include "rex_directx/utility/d3dx12.h"
#include "rex_directx/diagnostics/dx_call.h"
#include "rex_directx/system/dx_feature_level.h"
#include "rex_directx/system/dx_feature_shader_model.h"
#include "rex_directx/system/dx_swapchain.h"
#include "rex_directx/utility/dx_util.h" // IWYU pragma: keep
#include "rex_directx/dxgi/adapter.h"
#include "rex_directx/dxgi/adapter_manager.h"
#include "rex_directx/dxgi/factory.h"
#include "rex_directx/system/dx_rhi.h"
#include "rex_directx/dxgi/util.h"
#include "rex_directx/diagnostics/log.h"

#include "rex_directx/resources/dx_clear_state_resource.h"
#include "rex_directx/resources/dx_input_layout_resource.h"
#include "rex_directx/resources/dx_pipeline_state_resource.h"
#include "rex_directx/resources/dx_pixel_shader_resource.h"
#include "rex_directx/resources/dx_raster_state_resource.h"
#include "rex_directx/resources/dx_shader_program_resource.h"
#include "rex_directx/resources/dx_vertex_shader_resource.h"
#include "rex_directx/system/dx_device.h"
#include "rex_directx/system/dx_commandlist.h"

#include "rex_engine/platform/win/win_com_ptr.h"

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/memory/pointer_math.h"
#include "rex_engine/timing/execution_logger.h"

#include "rex_renderer_core/system/renderer.h"
#include "rex_renderer_core/system/renderer_backend.h"
#include "rex_renderer_core/rendering/cull_mode.h"
#include "rex_renderer_core/rendering/fill_mode.h"
#include "rex_renderer_core/system/gpu_description.h"
#include "rex_renderer_core/rendering/index_buffer_format.h"
#include "rex_renderer_core/rendering/input_layout_classification.h"
#include "rex_renderer_core/rendering/renderer_output_window_user_data.h"
#include "rex_renderer_core/resource_management/resource_pool.h"
#include "rex_renderer_core/rendering/scissor_rect.h"
#include "rex_renderer_core/shaders/shader_platform.h"
#include "rex_renderer_core/rendering/texture_format.h"
#include "rex_renderer_core/rendering/vertex_buffer_format.h"
#include "rex_renderer_core/rendering/viewport.h"
#include "rex_renderer_core/rendering/msaa_settings.h"
#include "rex_renderer_core/rendering/render_item.h"
#include "rex_renderer_core/resource_management/resource_slot.h"

#include "rex_std/algorithm.h"
#include "rex_std/bonus/memory/memory_size.h"
#include "rex_std/bonus/platform/windows/handle.h"
#include "rex_std/bonus/string.h"
#include "rex_std/bonus/utility/casting.h"
#include "rex_std/bonus/utility/enum_reflection.h"
#include "rex_std/bonus/utility/scopeguard.h"
#include "rex_std/memory.h"
#include "rex_std/vector.h"

#include <optional>

#include <D3Dcompiler.h>
#include <DirectXColors.h>
#include <Windows.h>
#include <cstddef>
#include <d3d12.h>

#ifdef REX_ENABLE_DXGI_DEBUG_LAYER
  #include <dxgidebug.h>
#endif

namespace rex
{
  namespace renderer
  {
    namespace directx
    {
      struct DefaultBuffer
      {
          wrl::ComPtr<ID3D12Resource> buffer;
          wrl::ComPtr<ID3D12Resource> upload_buffer;
      };

    } // namespace directx

    //-------------------------------------------------------------------------
    ShaderPlatform shader_platform()
    {
      return ShaderPlatform::HLSL;
    }

    //-------------------------------------------------------------------------
    bool is_y_up()
    {
      return false;
    }
    //-------------------------------------------------------------------------
    bool is_depth_0_to_1()
    {
      return true;
    }

    namespace backend
    {
      class DirectXRenderer
      {
      public:
        DirectXRenderer(const OutputWindowUserData& userData)
          : init_successful(true)
        {
          // Create a scopeguard so if we exit the renderer too early on
          // We mark it as initialization failed
          rsl::scopeguard mark_init_failed = [this]() { init_successful = false; };

          // Init the clear state
          if (!init_clear_state())
          {
            REX_ERROR(LogDirectX, "Failed to create the clear state");
            return;
          }

          init_viewport(userData);
          init_scirssor_rect(userData);

          // release the scopeguard so that init gets marked successful
          mark_init_failed.release();
        }

      private:
        // the global clear state by the renderer
        // If a clear command doesn't have a clear state defined
        // This will be the clear state that we use, which clears everything
        bool init_clear_state()
        {
          rhi::ClearStateDesc desc{};

          desc.rgba = rsl::colors::LightSteelBlue;
          desc.depth = 1.0f;
          desc.stencil = 0x00;
          desc.flags.add_state(ClearBits::ClearColorBuffer);
          desc.flags.add_state(ClearBits::ClearDepthBuffer);
          desc.flags.add_state(ClearBits::ClearStencilBuffer);

          clear_state = rhi::create_clear_state(desc);

          return true;
        }
        void init_viewport(const OutputWindowUserData& userData)
        {
          // Create viewport to render our image in
          // A viewport always needs to reset whenever a command list is reset
          screen_viewport.TopLeftX = 0.0f;
          screen_viewport.TopLeftY = 0.0f;
          screen_viewport.Width = static_cast<f32>(userData.window_width);
          screen_viewport.Height = static_cast<f32>(userData.window_height);
          screen_viewport.MinDepth = 0.0f;
          screen_viewport.MaxDepth = 1.0f;
        }
        void init_scirssor_rect(const OutputWindowUserData& userData)
        {
          // Cull pixels drawn outside of the backbuffer ( such as UI elements )
          scissor_rect = { 0, 0, static_cast<s32>(userData.window_width), static_cast<s32>(userData.window_height) };
        }

      public:
        rhi::ResourceSlot clear_state; // the default clear state

        D3D12_VIEWPORT screen_viewport; // The viewport pointing to the entire window
        RECT scissor_rect; // The scissor rect pointing to the entire window
        rsl::vector<RenderItem> render_items;

        rhi::ResourceSlot active_pso;

        bool init_successful;
      };
      
      rsl::unique_ptr<DirectXRenderer> g_renderer;

      //-------------------------------------------------------------------------
      bool initialize(const OutputWindowUserData& userData)
      {
        // Initialize the render hardware interface
        // This is the first layer of abstraction between the hardware
        // and the software.
        {
          ExecutionLogger exec_logger(LogDirectX, "Render Hardware Infrastructure Initialization");
          if (!rhi::init(userData))
          {
            REX_ERROR(LogDirectX, "Failed to initialize rhi layer.");
            return false;
          }
        }

        // After the rhi is initialized, initialize the renderer
        // The renderer will call into the rhi to create various resources
        // Therefore the rhi needs to get initialized first.
        g_renderer = rsl::make_unique<DirectXRenderer>(userData);

        // Initialization of the renderer could fail
        // Therefore lets guard here and let the application handle 
        // the initialization failure.
        if (!g_renderer->init_successful)
        {
          REX_ERROR(LogDirectX, "Failed to initialize the renderer");
          return false;
        }

        // The renderer is fully initialized from here on out
        // All systems go and rendering can be done.
        return true;
      }

      void render()
      {
        for (const auto& render_item : g_renderer->render_items)
        {
          rhi::set_vertex_buffer(render_item.vb());
          rhi::set_index_buffer(render_item.ib());
          rhi::set_constant_buffer(0, render_item.cb());
          rhi::set_primitive_topology(render_item.primtive_topology());

          rhi::draw_indexed(1, 0, render_item.index_count(), render_item.start_index(), render_item.base_vertex_loc());
        }
      }

      void shutdown()
      {
        g_renderer.reset();
        rhi::shutdown();
      }

      RenderItem* add_render_item(const RenderItemDesc& desc)
      {
        // 1) First we need to create the gpu resources for this render item on the gpu
        rhi::ResourceSlot vb = rhi::create_vertex_buffer(desc.vb_desc);
        rhi::ResourceSlot ib = rhi::create_index_buffer(desc.ib_desc);
        rhi::ResourceSlot cb = rhi::create_constant_buffer(desc.cb_desc);

        // 2) Next we need to make sure we have the correct views to these resource, so we can use them for rendering
        return &g_renderer->render_items.emplace_back(vb, ib, cb, desc.topology, desc.ib_desc.index_count);
      }
      
      //-------------------------------------------------------------------------
      s32 max_frames_in_flight()
      {
        // Nothing to implement
        return 0;
      }

      //-------------------------------------------------------------------------
      bool prepare_user_initialization()
      {
        // Nothing to implement

        return true;
      }

      //-------------------------------------------------------------------------
      bool finish_user_initialization()
      {
        // Nothing to implement

        return true;
      }

      //-------------------------------------------------------------------------
      bool set_viewport(const Viewport& viewport)
      {
        rhi::set_viewport(viewport);

        return true;
      }

      //-------------------------------------------------------------------------
      bool set_scissor_rect(const ScissorRect& rect)
      {
        rhi::set_scissor_rect(rect);

        return true;
      }

      //-------------------------------------------------------------------------
      bool new_frame()
      {
        rhi::reset_command_list(g_renderer->active_pso);
        rhi::reset_upload_buffer();

        return true;
      }

      //-------------------------------------------------------------------------
      bool end_frame()
      {
        // Nothing to implement

        return true;
      }

      //-------------------------------------------------------------------------
      bool begin_draw()
      {
        rhi::swap_rendertargets();

        rhi::transition_backbuffer(D3D12_RESOURCE_STATE_RENDER_TARGET);

        rhi::clear_backbuffer(g_renderer->clear_state);

        return true;
      }

      //-------------------------------------------------------------------------
      bool end_draw()
      {
        rhi::transition_backbuffer(D3D12_RESOURCE_STATE_PRESENT);

        rhi::exec_command_list();
        rhi::flush_command_queue();

        return true;
      }

      //-------------------------------------------------------------------------
      bool present()
      {
        rhi::present();

        return true;
      }
    } // namespace backend
  }   // namespace renderer
} // namespace rex