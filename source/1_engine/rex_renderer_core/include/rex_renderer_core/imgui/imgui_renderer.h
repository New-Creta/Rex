#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/diagnostics/error.h"
#include "rex_renderer_core/resources/pipeline_state.h"
#include "rex_renderer_core/resources/input_layout.h"
#include "rex_renderer_core/system/resource_state_tracker.h"
#include "rex_renderer_core/resources/raster_state.h"
#include "rex_renderer_core/rendering/renderer.h"

namespace rex
{
  namespace rhi
  {
    class Texture2D;
    class RootSignature;
    class InputLayout;
    class ConstantBuffer;
    class PipelineState;
    class RasterStateResource;
    class Shader;
  }

  class ImGuiRenderer : public Renderer
  {
  public:
    ImGuiRenderer(void* platformWindowHandle);

    void new_frame() override;
    void render() override;

  private:
    void init_imgui();
    void init_platform(void* platformWindowHandle);
    void init_gpu_resources();
    void init_main_imgui_viewport();

    void init_font_texture();
    void init_shader();
    void init_root_signature();
    void init_input_layout();
    void init_pso();
    void init_imgui_renderstate();

  private:
    rsl::unique_ptr<rhi::Texture2D> m_fonts_texture;
    rsl::unique_ptr<rhi::Shader> m_vertex_shader;
    rsl::unique_ptr<rhi::Shader> m_pixel_shader;
    rsl::unique_ptr<rhi::RootSignature> m_root_signature;
    rsl::unique_ptr<rhi::InputLayout> m_input_layout;
    rsl::unique_ptr<rhi::RasterStateResource> m_raster_state;
    rsl::unique_ptr<rhi::PipelineState> m_pipeline_state;
  };

  bool imgui_platform_init(void* platformWindowHandle);
  void imgui_platform_new_frame();
}