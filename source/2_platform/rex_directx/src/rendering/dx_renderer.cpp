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
#include "rex_directx/rendering/dx_imgui_renderer2.h"

#include "imgui/platform/win/imgui_impl_win32.h"

#ifdef REX_ENABLE_DXGI_DEBUG_LAYER
  #include <dxgidebug.h>
#endif

namespace rex
{
  namespace renderer
  {
    //-------------------------------------------------------------------------
    ShaderPlatform shader_platform()
    {
      return ShaderPlatform::HLSL;
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
      struct PassConstants
      {
        glm::mat4 view          = glm::mat4(1.0f);
        glm::mat4 inv_view      = glm::mat4(1.0f);
        glm::mat4 proj          = glm::mat4(1.0f);
        glm::mat4 inv_proj      = glm::mat4(1.0f);
        glm::mat4 view_proj     = glm::mat4(1.0f);
        glm::mat4 inv_view_proj = glm::mat4(1.0f);

        glm::vec3 eye_pos_w = {0.0f, 0.0f, 0.0f};
        f32 cb_padding_1    = 0.0f;

        glm::vec2 render_target_size     = {0.0f, 0.0f};
        glm::vec2 inv_render_target_size = {0.0f, 0.0f};

        f32 near_z = 0.0f;
        f32 far_z  = 0.0f;

        f32 total_time = 0.0f;
        f32 delta_time = 0.0f;
      };

      // The renderer is just a wrapper around resources
      // It does not handle any rendering logic itself
      // This is so the resources the renderer wraps can be deleted
      // using RAII without any manual involvement
      class DirectXRenderer
      {
      public:
        explicit DirectXRenderer(const OutputWindowUserData& userData)
            : init_successful(true)
            , depth_info({1.0f, 1000.0f})
            , screen_viewport()
            , scissor_rect()
        {
          // Create a scopeguard so if we exit the renderer too early on
          // We mark it as initialization failed
          rsl::scopeguard mark_init_failed = [this]() { init_successful = false; };

          // Create a commands frame, this makes sure all the commands get executed at the end of the scope
          const rhi::CommandsFrame cmds_frame {};

          // Init the clear state
          if(!init_clear_state())
          {
            REX_ERROR(LogDirectX, "Failed to create the clear state");
            return;
          }

          init_viewport(userData);
          init_scissor_rect(userData);
          init_pass_constants(userData);

          // release the scopeguard so that init gets marked successful
          mark_init_failed.release();
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

          clear_state = rhi::create_clear_state(desc);

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
        void init_pass_constants(const OutputWindowUserData& userData)
        {
          const f32 width  = static_cast<f32>(userData.window_width);
          const f32 height = static_cast<f32>(userData.window_height);

          pass_constants.eye_pos_w.x = 15.0f * sinf(0.2f * glm::pi<f32>()) * cosf(1.5f * glm::pi<f32>());
          pass_constants.eye_pos_w.y = 15.0f * cosf(0.2f * glm::pi<f32>());
          pass_constants.eye_pos_w.z = 35.0f * sinf(0.2f * glm::pi<f32>()) * sinf(1.5f * glm::pi<f32>());

          const glm::vec3 pos    = glm::vec3(pass_constants.eye_pos_w.x, pass_constants.eye_pos_w.y, pass_constants.eye_pos_w.z);
          const glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
          const glm::vec3 up     = glm::vec3(0.0f, 1.0f, 0.0f);

          glm::mat4 view = glm::lookAt(pos, target, up);
          view           = glm::transpose(view); // DirectX backend ( so we have to transpose, expects row major matrices )

          glm::mat4 proj = glm::perspectiveFov(0.25f * glm::pi<f32>(), width, height, depth_info.near_plane, depth_info.far_plane);
          proj           = glm::transpose(proj); // DirectX backend ( so we have to transpose, expects row major matrices )

          const glm::mat4 view_proj = view * proj;

          const glm::mat4 inv_view      = glm::inverse(view);
          const glm::mat4 inv_proj      = glm::inverse(proj);
          const glm::mat4 inv_view_proj = glm::inverse(view_proj);

          pass_constants.view          = view;
          pass_constants.inv_view      = inv_view;
          pass_constants.proj          = proj;
          pass_constants.inv_proj      = inv_proj;
          pass_constants.view_proj     = view_proj;
          pass_constants.inv_view_proj = inv_view_proj;

          pass_constants.render_target_size     = glm::vec2(width, height);
          pass_constants.inv_render_target_size = glm::vec2(1.0f / width, 1.0f / height);
          pass_constants.near_z                 = depth_info.near_plane;
          pass_constants.far_z                  = depth_info.far_plane;
          pass_constants.delta_time             = 0.0f;

          rex::memory::Blob pass_cb_blob(rsl::make_unique<rsl::byte[]>(sizeof(pass_constants))); // NOLINT(modernize-avoid-c-arrays)
          pass_cb_blob.write(&pass_constants, sizeof(pass_constants));
          rhi::ConstantBufferDesc desc;
          desc.blob_view       = rex::memory::BlobView(pass_cb_blob);
          pass_constant_buffer = rhi::create_constant_buffer(desc);
        }

      public:
        rhi::ResourceSlot clear_state;          // the default clear state
        rhi::ResourceSlot pass_constant_buffer; // Constant buffer used per rendering pass
        Viewport screen_viewport;         // The viewport pointing to the entire window
        ScissorRect scissor_rect;                      // The scissor rect pointing to the entire window
        rsl::vector<RenderItem> render_items;
        PassConstants pass_constants;
        DepthInfo depth_info;
        rhi::ResourceSlot active_pso;

        bool init_successful;
      };

      rsl::unique_ptr<DirectXRenderer> g_renderer; // NOLINT(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)
      //rsl::unique_ptr<imgui::ImGuiRenderer> g_imgui_renderer;

      //-------------------------------------------------------------------------
      bool initialize(const OutputWindowUserData& userData)
      {
        // Initialize the render hardware interface
        // This is the first layer of abstraction between the hardware
        // and the software.
        {
          const ExecutionLogger exec_logger(LogDirectX, "Render Hardware Infrastructure Initialization");
          if(!rhi::init(userData))
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
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable gamepad controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable multi-viewport / Platform windows
        
        // Enable dark mode
        ImGui::StyleColorsDark();
        
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
          style.WindowRounding = 0.0f;
          style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }
        
        io.BackendRendererName = "DirectX 12 ImGui Renderer";
        io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

        rhi::reset_command_list(rhi::ResourceSlot::make_invalid());
        ImGui_ImplWin32_Init((HWND)userData.primary_display_handle);
        ImGui_ImplDX12_Init(rhi::get_device(), 3, DXGI_FORMAT_R8G8B8A8_UNORM, rhi::get_cbv_uav_srv_heap());
        rhi::exec_command_list();
        rhi::flush_command_queue();

        // The renderer is fully initialized from here on out
        // All systems go and rendering can be done.
        return true;
      }

      void update_pass_constants()
      {
        g_renderer->pass_constants.delta_time = globals::frame_info().delta_time().to_milliseconds();
        rhi::update_buffer(g_renderer->pass_constant_buffer, &g_renderer->pass_constants, sizeof(g_renderer->pass_constants));
      }

      void render()
      {
        update_pass_constants();

        for(const auto& render_item: g_renderer->render_items)
        {
          rhi::set_vertex_buffer(render_item.vb());
          rhi::set_index_buffer(render_item.ib());
          rhi::set_constant_buffer(0, render_item.cb());
          rhi::set_constant_buffer(1, g_renderer->pass_constant_buffer);

          rhi::set_primitive_topology(render_item.primtive_topology());

          rhi::draw_indexed(1, 0, render_item.index_count(), render_item.start_index(), render_item.base_vertex_loc());
        }

        ImGui_ImplWin32_NewFrame();
        ImGui_ImplDX12_NewFrame();
        ImGui::NewFrame();

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

        ImGui::Render();
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), rhi::cmd_list());

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
          ImGui::UpdatePlatformWindows();
          ImGui::RenderPlatformWindowsDefault(nullptr, (void*)rhi::cmd_list());
        }


        //ImGui::EndFrame();
      }

      void shutdown()
      {
        g_renderer.reset();
        rhi::shutdown();
      }

      RenderItem* add_render_item(const RenderItemDesc& desc)
      {
        // 1) First we need to create the gpu resources for this render item on the gpu
        rhi::ResourceSlot vb           = rhi::create_vertex_buffer(desc.vb_desc);
        rhi::ResourceSlot ib           = rhi::create_index_buffer(desc.ib_desc);
        rhi::ResourceSlot cb           = rhi::create_constant_buffer(desc.cb_desc);

        // 2) Next we need to make sure we have the correct views to these resource, so we can use them for rendering
        RenderItem* render_item = &g_renderer->render_items.emplace_back(vb, ib, cb, desc.topology, desc.ib_desc.index_count, desc.ib_desc.base_vertex_loc);

        // 3) Make sure the constant buffer gets uploaded
        rex::rhi::update_buffer(render_item->cb(), desc.cb_desc.blob_view.data(), desc.cb_desc.blob_view.size());

        // 4) return the new added render item
        return render_item;
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

        set_viewport(g_renderer->screen_viewport);
        set_scissor_rect(g_renderer->scissor_rect);

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
        rhi::bind_backbuffer_rendertarget();

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