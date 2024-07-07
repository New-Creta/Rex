#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/diagnostics/error.h"
#include "rex_renderer_core/resources/pipeline_state.h"
#include "rex_renderer_core/resources/input_layout.h"
#include "rex_renderer_core/system/resource_state_tracker.h"
#include "rex_renderer_core/resources/raster_state.h"
#include "rex_renderer_core/rendering/renderer.h"
#include "rex_renderer_core/materials/material.h"

namespace rex
{
  namespace gfx
  {
    class Texture2D;
    class RootSignature;
    class InputLayout;
    class PipelineState;
    class RasterState;
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

      // GPU resource initialization
      void init_font_texture();
      void init_font_sampler();
      void init_shader();
      void init_material();
      void init_root_signature();
      void init_input_layout();
      void init_pso();
      void init_imgui_renderstate();

      // destroy imgui
      void destroy_viewports();

    private:
      rsl::unique_ptr<Texture2D> m_fonts_texture;          // The fonts texture, the data comes from imgui
      ShaderSamplerDesc m_fonts_sampler;                   // The sampler for the fonts texture
      rsl::unique_ptr<Shader> m_vertex_shader;             // The vertex shader used by imgui
      rsl::unique_ptr<Shader> m_pixel_shader;              // The pixel shader used by imgui
      //rsl::unique_ptr<RootSignature> m_root_signature;     // The root signature used by imgui, this will become a material in the future
      rsl::unique_ptr<InputLayout> m_input_layout;         // The input layout used by imgui
      rsl::unique_ptr<RasterState> m_raster_state; // The render state used by imgui
      rsl::unique_ptr<PipelineState> m_pipeline_state;     // The pipeline state used by imgui
      rsl::unique_ptr<Material> m_material;                // The material used by imgui
    };

    // Perform platform specific imgui initialization, this is not defined here.
    bool imgui_platform_init(void* platformWindowHandle);
    // destroy all platform specific imgui data
    void imgui_platform_shutdown();
    // Perform platform specific preparations for a new frame
    void imgui_platform_new_frame();
  }
}