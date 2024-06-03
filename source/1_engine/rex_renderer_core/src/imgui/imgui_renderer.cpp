#include "rex_renderer_core/imgui/imgui_renderer.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/memory/global_allocator.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"

#include "rex_renderer_core/rhi/rhi.h"
#include "rex_renderer_core/resources/texture_2d.h"
#include "rex_renderer_core/rhi/texture_format.h"

#include "rex_renderer_core/imgui/imgui_frame_context.h"
#include "rex_renderer_core/imgui/imgui_viewport.h"

#include "rex_renderer_core/gfx/graphics.h"

#include "imgui/imgui.h"

namespace rex
{
  DEFINE_LOG_CATEGORY(LogImgui);

  ImGuiRenderer::ImGuiRenderer(void* platformWindowHandle)
  {
    init_imgui();
    init_platform(platformWindowHandle);
    init_gpu_resources();
    init_main_imgui_viewport();
  }

  void ImGuiRenderer::new_frame()
  {
    imgui_platform_new_frame();

    ImGui::NewFrame();

    REX_STATIC_WARNING("Get a new render target for the renderer to render to");
  }

  void ImGuiRenderer::render()
  {
    ImGui::Render();

    auto render_ctx = gfx::new_render_ctx();

    ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    if (RexImGuiViewport* imgui_window = (RexImGuiViewport*)main_viewport->RendererUserData)
    {
      imgui_window->render(*render_ctx);
    }

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault(nullptr, nullptr);
    }
  }

  void ImGuiRenderer::init_imgui()
  {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable gamepad controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable multi-viewport / Platform windows

    // Enable dark mode
    ImGui::StyleColorsDark();

    // We support multiple viewport, having imgui widget in their own windows when dragged outside of the main window
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
      style.WindowRounding = 0.0f;
      style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    io.BackendRendererName = "DirectX 12 ImGui Renderer";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

    IM_ASSERT(io.BackendRendererUserData == nullptr && "Already initialized a renderer backend!");

    // Setup backend capabilities flags
    io.BackendRendererUserData = (void*)this;
    io.BackendRendererName = "ImGui Renderer";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
    io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;  // We can create multi-viewports on the Renderer side (optional)
  }

  void ImGuiRenderer::init_platform(void* platformWindowHandle)
  {
    imgui_platform_init(platformWindowHandle);
  }

  void ImGuiRenderer::init_gpu_resources()
  {
    init_font_texture();
    init_shader();
    init_input_layout();
    init_constant_buffer();
    init_root_signature();
    init_pso();
  }

  void ImGuiRenderer::init_main_imgui_viewport()
  {
    ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGuiRenderState render_state{};
    render_state.root_signature = m_root_signature.get();
    render_state.pso = m_pipeline_state.get();
    render_state.constant_buffer = m_constant_buffer.get();
    render_state.primitive_topology = renderer::PrimitiveTopology::TriangleList;
    render_state.blend_factor = { 0.0f, 0.0f, 0.0f, 0.0f };
    main_viewport->RendererUserData = rex::debug_alloc<RexImGuiViewport>(main_viewport, render_state);
  }

  void ImGuiRenderer::init_font_texture()
  {
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    s32 width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    renderer::TextureFormat format = renderer::TextureFormat::Unorm4;

    m_fonts_texture = rhi::create_texture2d(width, height, format, pixels);
  }
  void ImGuiRenderer::init_shader()
  {
    rex::memory::Blob vertex_shader_content = vfs::read_file(path::join(vfs::engine_root(), "shaders", "imgui", rhi::shader_platform(), "imgui_vertex.hlsl"));
    rex::memory::Blob pixel_shader_content = vfs::read_file(path::join(vfs::engine_root(), "shaders", "imgui", rhi::shader_platform(), "imgui_pixel.hlsl"));

    m_vertex_shader = rhi::create_vertex_shader(blob_to_string_view(vertex_shader_content));
    m_pixel_shader = rhi::create_pixel_shader(blob_to_string_view(pixel_shader_content));
  }
  void ImGuiRenderer::init_root_signature()
  {
    // The following should be auto generated from the shaders using reflection
    // That'd allow code to look like this
    //
    // rex::rhi::RootSignatureDesc root_sig_desc;
    // root_sig_desc.vertex_shader = ..;
    // root_sig_desc.pixel_shader = ..l
    // ..
    // m_root_signature = rhi::create_root_signature(root_sig_desc);
    //
    // Which makes it much simpler to use

    rex::rhi::RootSignatureDesc root_sig_desc{};
    // We have 2 constants for the shader, 1 in the vertex shader and 1 in the pixel shader
    root_sig_desc.views = rsl::make_unique<rex::rhi::ShaderViewDesc[]>(1);
    root_sig_desc.views[0] = { "vertexBuffer", rhi::ShaderViewType::ConstantBufferView, 0, 0, rex::renderer::ShaderVisibility::Vertex }; // We have 1 constant buffer in the vertex shader

    root_sig_desc.desc_tables = rsl::make_unique<rex::rhi::DescriptorTableDesc[]>(1);
    root_sig_desc.desc_tables[0].ranges = rsl::make_unique<rex::rhi::DescriptorRangeDesc[]>(1);
    root_sig_desc.desc_tables[0].ranges[0] = { rex::rhi::DescriptorRangeType::ShaderResourceView, 1 }; // We have 1 src which points to our font texture
    root_sig_desc.desc_tables[0].visibility = rex::renderer::ShaderVisibility::Pixel;

    // We have 1 sampler, used for sampling the font texture
    root_sig_desc.samplers = rsl::make_unique<rex::rhi::ShaderSamplerDesc[]>(1);
    root_sig_desc.samplers[0].filtering = rex::renderer::SamplerFiltering::MinMagMipLinear;
    root_sig_desc.samplers[0].address_mode_u = rex::renderer::TextureAddressMode::Wrap;
    root_sig_desc.samplers[0].address_mode_v = rex::renderer::TextureAddressMode::Wrap;
    root_sig_desc.samplers[0].address_mode_w = rex::renderer::TextureAddressMode::Wrap;
    root_sig_desc.samplers[0].mip_lod_bias = 0.0f;
    root_sig_desc.samplers[0].max_anisotropy = 0;
    root_sig_desc.samplers[0].comparison_func = rex::renderer::ComparisonFunc::Always;
    root_sig_desc.samplers[0].border_color = rex::renderer::BorderColor::TransparentBlack;
    root_sig_desc.samplers[0].min_lod = 0.0f;
    root_sig_desc.samplers[0].max_lod = 0.0f;
    root_sig_desc.samplers[0].shader_register = 0;
    root_sig_desc.samplers[0].register_space = 0;
    root_sig_desc.samplers[0].shader_visibility = rex::renderer::ShaderVisibility::Pixel;

    m_root_signature = rex::rhi::create_root_signature(root_sig_desc);
  }
  void ImGuiRenderer::init_input_layout()
  {
    // The following should be auto genrated from the shaders using reflection
    // That'd allow code to look like this
    //
    // rex::rhi::InputLayoutDesc input_layout_desc;
    // input_layout_desc.vertex_shader = ..;
    // ..
    // m_input_layout = rhi::create_input_layout(input_layout_desc);

    rex::rhi::InputLayoutDesc input_layout_desc;
    input_layout_desc.input_layout =
    {
      rex::rhi::InputLayoutElementDesc { "POSITION",  rex::renderer::VertexBufferFormat::Float2, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 0, 0 },
      rex::rhi::InputLayoutElementDesc { "TEXCOORD",  rex::renderer::VertexBufferFormat::Float2, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 8, 0 },
      rex::rhi::InputLayoutElementDesc { "COLOR", rex::renderer::VertexBufferFormat::UNorm4, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 16, 0 }
    };
    m_input_layout = rex::rhi::create_input_layout(input_layout_desc);
  }
  void ImGuiRenderer::init_constant_buffer()
  {
    // The following should be auto generated from the shaders using reflection
    // That'd allow code to look like this
    //
    // m_constant_buffer = rex::rhi::create_constant_buffer(shader_object, "constant_buffer_name");

    m_constant_buffer = rex::rhi::create_constant_buffer(sizeof(ImGuiVertexConstantBuffer));
  }
  void ImGuiRenderer::init_pso()
  {
    // The following should be generated from a material.
    // The material gets loaded from disk, which then initializes the pso

     // Raster State
    rex::rhi::RasterStateDesc rasterizer_desc{};
    rasterizer_desc.fill_mode = rex::renderer::FillMode::Solid;
    rasterizer_desc.cull_mode = rex::renderer::CullMode::None;
    rasterizer_desc.front_ccw = false;
    rasterizer_desc.depth_bias = 0;
    rasterizer_desc.depth_bias_clamp = 0.0f;
    rasterizer_desc.sloped_scale_depth_bias = 0.0f;
    rasterizer_desc.depth_clip_enable = true;
    rasterizer_desc.multisample = false;
    rasterizer_desc.aa_lines = false;
    rasterizer_desc.forced_sample_count = 0;
    m_raster_state = rex::rhi::create_raster_state(rasterizer_desc);

    rex::rhi::PipelineStateDesc pso_desc{};
    pso_desc.input_layout = m_input_layout.get();
    pso_desc.raster_state = *m_raster_state.get();
    pso_desc.vertex_shader = m_vertex_shader.get();
    pso_desc.pixel_shader = m_pixel_shader.get();
    pso_desc.root_signature = m_root_signature.get();

    // Blend State
    pso_desc.blend_state = rex::rhi::BlendDesc();
    rex::rhi::BlendDesc& blend_state = pso_desc.blend_state.value();
    blend_state.enable_alpha_to_coverage = false;
    blend_state.render_target[0].blend_enable = true;
    blend_state.render_target[0].src_blend = rex::rhi::Blend::SrcAlpha;
    blend_state.render_target[0].dst_blend = rex::rhi::Blend::InvSrcAlpha;
    blend_state.render_target[0].blend_op = rex::rhi::BlendOp::Add;
    blend_state.render_target[0].src_blend_alpha = rex::rhi::Blend::One;
    blend_state.render_target[0].dst_blend_alpha = rex::rhi::Blend::InvSrcAlpha;
    blend_state.render_target[0].blend_op_alpha = rex::rhi::BlendOp::Add;
    blend_state.render_target[0].render_target_write_mask = rex::rhi::RenderTargetWriteMask::All;

    // depth stencil state
    pso_desc.depth_stencil_state = rex::rhi::DepthStencilDesc();
    rex::rhi::DepthStencilDesc& depth_stencil_desc = pso_desc.depth_stencil_state.value();
    depth_stencil_desc.depth_enable = false;
    depth_stencil_desc.depth_write_mask = rex::rhi::DepthWriteMask::DepthWriteMaskAll;
    depth_stencil_desc.depth_func = rex::renderer::ComparisonFunc::Always;
    depth_stencil_desc.stencil_enable = false;
    depth_stencil_desc.front_face.stencil_fail_op = rex::rhi::StencilOp::Keep;
    depth_stencil_desc.front_face.stencil_depth_fail_op = rex::rhi::StencilOp::Keep;
    depth_stencil_desc.front_face.stencil_pass_op = rex::rhi::StencilOp::Keep;
    depth_stencil_desc.front_face.stencil_func = rex::renderer::ComparisonFunc::Always;
    depth_stencil_desc.back_face = depth_stencil_desc.front_face;

    m_pipeline_state = rex::rhi::create_pso(pso_desc);
  }
}