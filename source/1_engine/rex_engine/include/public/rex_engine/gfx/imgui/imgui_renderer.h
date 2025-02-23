#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/diagnostics/error.h"
#include "rex_engine/gfx/resources/pipeline_state.h"
#include "rex_engine/gfx/resources/input_layout.h"
#include "rex_engine/gfx/system/resource_state_tracker.h"
#include "rex_engine/gfx/resources/raster_state.h"
#include "rex_engine/gfx/rendering/renderer.h"
#include "rex_engine/gfx/rendering/render_pass.h"


namespace rex
{
  namespace gfx
  {
    class Texture2D;
    class Sampler2D;
    class InputLayout;
    class PipelineState;
    class Shader;

    class ImGuiRenderer : public Renderer
    {
    public:
      ImGuiRenderer(void* platformWindowHandle);
      ~ImGuiRenderer();

      // Prepare a new frame for imgui, enabling users to queue imgui objects
      void new_frame() override;
      // Render the queued imgui objects
      void render() override;

    private:
      // Init imgui itself
      void init_imgui(void* platformWindowHandle);
      // Init the gpu resources needed for imgui rendering
      void init_gpu_resources();
      // Init the main viewport of imgui. 
      // The main viewport is the one pointing to the main window of the application
      void init_main_imgui_viewport();
      // Init the colors for imgui to use
      void init_colors();

      // GPU resource initialization
      void init_font_texture();
      void init_font_sampler();
      void init_renderpass();

      // destroy imgui
      void destroy_viewports();

    private:
      rsl::unique_ptr<Texture2D> m_fonts_texture;          // The fonts texture, the data comes from imgui
      rsl::unique_ptr<Sampler2D> m_fonts_sampler;          // The sampler for the fonts texture
      rsl::unique_ptr<RenderPass> m_imgui_renderpass;      // The render pass used by imgui
    };

    // Perform platform specific imgui initialization, this is not defined here.
    bool imgui_platform_init(void* platformWindowHandle);
    // destroy all platform specific imgui data
    void imgui_platform_shutdown();
    // Perform platform specific preparations for a new frame
    void imgui_platform_new_frame();
  }
}