#include "rex_directx/diagnostics/dx_call.h"
#include "rex_directx/diagnostics/log.h"
#include "rex_directx/dxgi/adapter.h"
#include "rex_directx/dxgi/adapter_manager.h"
#include "rex_directx/dxgi/factory.h"
#include "rex_directx/dxgi/util.h"
#include "rex_directx/resources/dx_clear_state_resource.h"
#include "rex_directx/resources/dx_input_layout_resource.h"
#include "rex_directx/resources/dx_pixel_shader_resource.h"
#include "rex_directx/resources/dx_raster_state_resource.h"
#include "rex_directx/resources/dx_shader_program_resource.h"
#include "rex_directx/resources/dx_vertex_shader_resource.h"
#include "rex_directx/system/dx_commandlist.h"
#include "rex_directx/system/dx_command_allocator.h"
#include "rex_directx/system/dx_command_queue.h"
#include "rex_directx/system/dx_device.h"
#include "rex_directx/system/dx_feature_level.h"
#include "rex_directx/system/dx_feature_shader_model.h"
#include "rex_directx/system/dx_rhi.h"
#include "rex_directx/system/dx_swapchain.h"
#include "rex_directx/utility/d3dx12.h"
#include "rex_directx/utility/dx_util.h" // IWYU pragma: keep
#include "rex_engine/app/core_application.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/memory/pointer_math.h"
#include "rex_engine/platform/win/win_com_ptr.h"
#include "rex_engine/timing/execution_logger.h"
#include "rex_renderer_core/rendering/cull_mode.h"
#include "rex_renderer_core/rendering/depth_info.h"
#include "rex_renderer_core/rendering/fill_mode.h"
#include "rex_renderer_core/rendering/index_buffer_format.h"
#include "rex_renderer_core/rendering/input_layout_classification.h"
#include "rex_renderer_core/rendering/msaa_settings.h"
#include "rex_renderer_core/rendering/render_item.h"
#include "rex_renderer_core/rendering/renderer_output_window_user_data.h"
#include "rex_renderer_core/rendering/scissor_rect.h"
#include "rex_renderer_core/rendering/texture_format.h"
#include "rex_renderer_core/rendering/vertex_buffer_format.h"
#include "rex_renderer_core/rendering/viewport.h"
#include "rex_renderer_core/resource_management/resource_pool.h"
#include "rex_renderer_core/resource_management/resource_slot.h"
#include "rex_renderer_core/shaders/shader_platform.h"
#include "rex_renderer_core/system/gpu_description.h"
#include "rex_renderer_core/system/renderer.h"
#include "rex_renderer_core/system/renderer_backend.h"
#include "rex_std/algorithm.h"
#include "rex_std/bonus/memory/memory_size.h"
#include "rex_std/bonus/platform/windows/handle.h"
#include "rex_std/bonus/string.h"
#include "rex_std/bonus/utility/casting.h"
#include "rex_std/bonus/utility/enum_reflection.h"
#include "rex_std/bonus/utility/scopeguard.h"
#include "rex_std/memory.h"
#include "rex_std/vector.h"

#include <D3Dcompiler.h>
#include <DirectXColors.h>
#include <Windows.h>
#include <cstddef>
#include <d3d12.h>
#include <glm/gtc/matrix_transform.hpp>
#include "rex_directx/rendering/dx_imgui_renderer.h"

#include "imgui/platform/win/imgui_impl_win32.h"

#ifdef REX_ENABLE_DXGI_DEBUG_LAYER
  #include <dxgidebug.h>
#endif

// I think it's fine for a renderer to control the resources. 
// But it should be done through and API similar to UE's RHI if you want to support more than one platform API (like Vulkan, OpenGL, etc.). 
// Otherwise it's going to be a pain in the ass to add that functionality later on.
//
// So I think the following architecture could make sense :
// 
// -- HIGH LEVEL RENDERING LIBRARY -- AKA -- Rex Renderer
// At the top you have a high level rendering library that manages the resource lifetimes, what happens in a frame, etc. 
// This will be used by the actual application that wants to render something. 
// Here you have most flexibility on what the API would look like.
// 
// -- RENDER HARDWARE INTERFACE -- AKA -- Rex RHI
// That rendering library would use a something similar to UE's RHI. 
// The RHI usually only supports a part of the things DX12 or Vulkan would support, as you are most likely not going to need all their features.
// 
// -- GRAPHICS API -- AKA -- DirectX12, Vulkan, OpenGL
//
// This means that a texture class is part of the rhi layer and will likely be called rhi::Texture2D,
// However, using this texture in the appropriate way is the renderer's responsibility
//
// The goal is that the renderer doesn't care which rhi we'll be using and the renderer itself should only be written once and not for every graphics api
// All graphics api code is abtracted by the rhi, so in fact, this piece of code should be taken out of the directx project and into the renderer code project
//

namespace rex
{
  namespace renderer
  {
    //-------------------------------------------------------------------------
    ShaderPlatform shader_platform()
    {
      return ShaderPlatform::Hlsl;
    }

    //-------------------------------------------------------------------------
    bool is_y_up()
    {
      return true;
    }
    //-------------------------------------------------------------------------
    bool is_depth_0_to_1()
    {
      return true;
    }

    namespace backend
    {
      // The renderer is just a wrapper around resources
      // It does not handle any rendering logic itself
      // This is so the resources the renderer wraps can be deleted
      // using RAII without any manual involvement
      class DirectXRenderer
      {
      public:
        explicit DirectXRenderer(const OutputWindowUserData& userData)
          : init_successful(true)
          , depth_info({ 1.0f, 1000.0f })
          , screen_viewport()
          , scissor_rect()
          , swapchain(rhi::create_swapchain(s_max_frames_in_flight, userData.primary_display_handle))
          , command_list(rhi::create_commandlist(&m_resource_state_tracker))
        {
          // Create a scopeguard so if we exit the renderer too early on
          // We mark it as initialization failed
          rsl::scopeguard mark_init_failed = [this]() { init_successful = false; };

          init_cmd_allocators();
          init_render_targets();

          // Init the clear state
          if(!init_clear_state())
          {
            REX_ERROR(LogDirectX, "Failed to create the clear state");
            return;
          }

          init_viewport(userData);
          init_scissor_rect(userData);

          // release the scopeguard so that init gets marked successful
          mark_init_failed.release();
        }

        ~DirectXRenderer()
        {
          // Make sure the gpu is done processing so we can safely shutdown
          rhi::wait_for_gpu(rhi::CommandType::Direct);
          rhi::wait_for_gpu(rhi::CommandType::Copy);
          rhi::wait_for_gpu(rhi::CommandType::Compute);
        }

      private:

        // the global clear state by the renderer
        // If a clear command doesn't have a clear state defined
        // This will be the clear state that we use, which clears everything
        bool init_clear_state()
        {
          rhi::ClearStateDesc desc {};

          desc.rgba    = rsl::colors::LightSteelBlue;
          desc.depth   = 1.0f;
          desc.stencil = 0x00;
          desc.flags.add_state(ClearBits::ClearColorBuffer);
          desc.flags.add_state(ClearBits::ClearDepthBuffer);
          desc.flags.add_state(ClearBits::ClearStencilBuffer);

          clear_state = rsl::make_unique<rhi::ClearStateResource>(desc);

          return true;
        }
        void init_viewport(const OutputWindowUserData& userData)
        {
          // Create viewport to render our image in
          // A viewport always needs to reset whenever a command list is reset
          const f32 width = static_cast<f32>(userData.window_width);
          const f32 height = static_cast<f32>(userData.window_height);
          screen_viewport = { 0.0f, 0.0f, width, height, 0.0f, 1.0f };
        }
        void init_scissor_rect(const OutputWindowUserData& userData)
        {
          // Cull pixels drawn outside of the backbuffer ( such as UI elements )
          scissor_rect = {0, 0, static_cast<f32>(userData.window_width), static_cast<f32>(userData.window_height)};
        }

        void init_cmd_allocators()
        {
          for (s32 i = 0; i < cmd_allocators.size(); ++i)
          {
            cmd_allocators[i] = rhi::create_command_allocator();
          }
        }
        void init_render_targets()
        {
          for (s32 i = 0; i < cmd_allocators.size(); ++i)
          {
            rhi::Resource2* buffer = swapchain->get_buffer(i);
            render_targets[i] = rhi::create_render_target_from_backbuffer(buffer);
          }
        }

      public:
        static constexpr s32 s_max_frames_in_flight = 2;

        rsl::unique_ptr<rhi::ClearStateResource> clear_state;          // the default clear state
        Viewport screen_viewport;               // The viewport pointing to the entire window
        ScissorRect scissor_rect;               // The scissor rect pointing to the entire window
        rsl::vector<RenderItem> render_items;   // The render items to draw
        DepthInfo depth_info;                   // Info about the depth settings (eg. near and far plane)
        rhi::ResourceSlot active_pso;           // The currently active pso
        bool init_successful;                   // Boolean indicating initialization was successful
        
        rsl::unique_ptr<rhi::Swapchain> swapchain;
        rsl::unique_ptr<rhi::CommandList> command_list;
        rhi::ResourceStateTracker m_resource_state_tracker;

        rsl::array<rsl::unique_ptr<rhi::CommandAllocator>, s_max_frames_in_flight> cmd_allocators;
        rsl::array<rsl::unique_ptr<rhi::RenderTarget>, s_max_frames_in_flight> render_targets;
      };

      rsl::unique_ptr<DirectXRenderer> g_renderer; // NOLINT(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)
      rsl::unique_ptr<ImGuiRenderer> g_imgui_renderer;

      //-------------------------------------------------------------------------
      bool initialize(const OutputWindowUserData& userData)
      {
        // Initialize the render hardware interface
        // This is the first layer of abstraction between the hardware
        // and the software.
        {
          const ExecutionLogger exec_logger(LogDirectX, "Render Hardware Infrastructure Initialization");
          if(!rhi::init(userData, DirectXRenderer::s_max_frames_in_flight))
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
        if(!g_renderer->init_successful)
        {
          REX_ERROR(LogDirectX, "Failed to initialize the renderer");
          return false;
        }

        // Initialize the imgui renderer if it's needed
        g_imgui_renderer = rsl::make_unique<ImGuiRenderer>(rhi::get_device(), 3, DXGI_FORMAT_R8G8B8A8_UNORM, (HWND)userData.primary_display_handle);

        // The renderer is fully initialized from here on out
        // All systems go and rendering can be done.
        return true;
      }

      void render()
      {
        rhi::global_upload_buffer()->reset();

        // Begin frame
        rhi::CommandAllocator* cmd_alloc = g_renderer->cmd_allocators[g_renderer->swapchain->get()->GetCurrentBackBufferIndex()].get();
        rhi::RenderTarget* render_target = g_renderer->render_targets[g_renderer->swapchain->get()->GetCurrentBackBufferIndex()].get();
        g_renderer->command_list->start_recording_commands(cmd_alloc);

        // Begin Draw
        g_renderer->command_list->set_viewport(g_renderer->screen_viewport);
        g_renderer->command_list->set_scissor_rect(g_renderer->scissor_rect);
        g_renderer->command_list->transition_buffer(render_target, ResourceState::RenderTarget);
        g_renderer->command_list->set_render_target(render_target);
        g_renderer->command_list->clear_render_target(render_target, g_renderer->clear_state.get());

        g_imgui_renderer->new_frame();

        ImGui::ShowDemoWindow();

        static bool show_another_window = true;
        if (show_another_window)
        {
          ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
          ImGui::Text("Hello from another window!");
          if (ImGui::Button("Close Me"))
            show_another_window = false;
          ImGui::End();
        }

        g_imgui_renderer->render(g_renderer->command_list.get());

        // End draw
        g_renderer->command_list->transition_buffer(render_target, ResourceState::Present);
        g_renderer->command_list->stop_recording_commands();

        // End Frame
        g_renderer->command_list->send_to_gpu();
        rhi::wait_for_gpu(rhi::CommandType::Direct);

        g_renderer->swapchain->present();
      }

      void shutdown()
      {
        g_renderer.reset();
        rhi::shutdown();
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
        return true;
      }

      //-------------------------------------------------------------------------
      bool set_viewport(const Viewport& viewport)
      {
        return true;
      }

      //-------------------------------------------------------------------------
      bool set_scissor_rect(const ScissorRect& rect)
      {
        return true;
      }

      //-------------------------------------------------------------------------
      bool new_frame()
      {
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
        return true;
      }

      //-------------------------------------------------------------------------
      bool end_draw()
      {
        return true;
      }

      //-------------------------------------------------------------------------
      bool present()
      {
        return true;
      }
    } // namespace backend
  }   // namespace renderer
} // namespace rex