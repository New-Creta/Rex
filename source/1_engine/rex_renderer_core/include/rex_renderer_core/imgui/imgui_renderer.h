#pragma once

#include "rex_engine/diagnostics/error.h"

namespace rex
{
  namespace rhi
  {
    class Texture2D;
    class VertexShader;
    class PixelShader;
    class RootSignature;
    class InputLayoutResource;
    class ConstantBuffer;
    class PipelineState;
    class RasterStateResource;
  }

  class ImGuiRenderer
  {
  public:
    ImGuiRenderer(void* platformWindowHandle);

    void new_frame();
    void render();

  private:
    void init_imgui();
    void init_platform(void* platformWindowHandle);
    void init_gpu_resources();
    void init_main_imgui_viewport();

    void init_font_texture();
    void init_shader();
    void init_root_signature();
    void init_input_layout();
    void init_constant_buffer();
    void init_pso();

  private:
    rsl::unique_ptr<rhi::Texture2D> m_fonts_texture;
    rsl::unique_ptr<rhi::VertexShader> m_vertex_shader;
    rsl::unique_ptr<rhi::PixelShader> m_pixel_shader;
    rsl::unique_ptr<rhi::RootSignature> m_root_signature;
    rsl::unique_ptr<rhi::InputLayoutResource> m_input_layout;
    rsl::unique_ptr<rhi::ConstantBuffer> m_constant_buffer;
    rsl::unique_ptr<rhi::RasterStateResource> m_raster_state;
    rsl::unique_ptr<rhi::PipelineState> m_pipeline_state;
  };

  bool imgui_platform_init(void* platformWindowHandle);
  void imgui_platform_new_frame();
}