#include "rex_directx/system/dx_renderer2.h"

#include "rex_renderer_core/rendering/renderer_output_window_user_data.h"
#include "rex_renderer_core/rendering/depth_info.h"
#include "rex_renderer_core/rendering/viewport.h"
#include "rex_renderer_core/rendering/scissor_rect.h"

#include "rex_renderer_core/resource_management/resource_slot.h"

#include "rex_directx/rendering/dx_imgui_renderer.h"
#include "rex_directx/system/dx_rhi.h"

#include "rex_engine/timing/execution_logger.h"
#include "rex_engine/diagnostics/log.h"

namespace rex
{
  namespace renderer
  {
    DEFINE_LOG_CATEGORY(LogDirectXRenderer);

    class DirectXRenderer
    {
    public:
      explicit DirectXRenderer(const OutputWindowUserData& userData)
        : init_successful(true)
        , depth_info({ 1.0f, 1000.0f })
        , screen_viewport()
        , scissor_rect()
      {}

    private:

    public:
      Viewport screen_viewport;               // The viewport pointing to the entire window
      ScissorRect scissor_rect;               // The scissor rect pointing to the entire window
      DepthInfo depth_info;                   // Info about the depth settings (eg. near and far plane)
      rhi::ResourceSlot clear_state;          // the default clear state
      rhi::ResourceSlot active_pso;           // The currently active pso
      bool init_successful;                   // Boolean indicating initialization was successful
    };

    rsl::unique_ptr<DirectXRenderer> g_renderer;
    rsl::unique_ptr<ImGuiRenderer> g_imgui_renderer;

    bool initialize(const OutputWindowUserData& userData)
    {
        // Initialize the render hardware interface
        // This is the first layer of abstraction between the hardware
        // and the software.
        {
          const ExecutionLogger exec_logger(LogDirectXRenderer, "Render Hardware Infrastructure Initialization");
          if(!rhi::init(userData))
          {
            REX_ERROR(LogDirectXRenderer, "Failed to initialize rhi layer.");
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
          REX_ERROR(LogDirectXRenderer, "Failed to initialize the renderer");
          return false;
        }

        // Initialize the imgui renderer so we can start drawing our development UI.
        // The game UI itself doesn't use imgui, but any debug UI does
        // Some tools also use imgui for their UI (eg. the editor)
        g_imgui_renderer = rsl::make_unique<ImGuiRenderer>(rhi::get_device(), 3, DXGI_FORMAT_R8G8B8A8_UNORM, (HWND)userData.primary_display_handle);

        // The renderer is fully initialized from here on out
        // all systems go and rendering can be done
        return true;
    }
    void render()
    {
      // Render our stuff here

      // Render imgui stuff next
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

      g_imgui_renderer->render();
    }
    void shutdown()
    {
      g_imgui_renderer.reset();
      g_renderer.reset();
      rhi::shutdown();
    }
  }
}