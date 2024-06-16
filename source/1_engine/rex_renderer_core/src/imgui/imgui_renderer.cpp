#include "rex_renderer_core/imgui/imgui_renderer.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/memory/global_allocator.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"

#include "rex_renderer_core/gfx/rhi.h"
#include "rex_renderer_core/resources/texture_2d.h"
#include "rex_renderer_core/gfx/texture_format.h"

#include "rex_renderer_core/imgui/imgui_frame_context.h"
#include "rex_renderer_core/imgui/imgui_viewport.h"
#include "rex_renderer_core/imgui/imgui_resources.h"

#include "rex_renderer_core/gfx/graphics.h"

#include "imgui/imgui.h"

namespace rex
{
  namespace gfx
  {

    ImGuiRenderer::ImGuiRenderer(void* platformWindowHandle)
    {
      init_imgui(platformWindowHandle);
      init_gpu_resources();
      init_main_imgui_viewport();

      // Open imgui to accept commands
      new_frame();
    }
    ImGuiRenderer::~ImGuiRenderer()
    {
      destroy_viewports();
      imgui_platform_shutdown();
      ImGui::DestroyContext();
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

      // As the imgui renderer main viewport is the main window of the application
      // it needs to set its render target to the one pointing to the current back buffer
      render_ctx->set_render_target(rhi::current_backbuffer_rt());

      ImGuiViewport* main_viewport = ImGui::GetMainViewport();
      if (RexImGuiViewport* rex_viewport = (RexImGuiViewport*)main_viewport->RendererUserData)
      {
        rex_viewport->render(*render_ctx);
      }

      // Update and render the imgui windows if any
      if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
      {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault(nullptr, nullptr);
      }

      // Open imgui to accept new commands again
      new_frame();
    }

    void ImGuiRenderer::init_imgui(void* platformWindowHandle)
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

      // Perform platform specific imgui initialization
      imgui_platform_init(platformWindowHandle);
    }

    void ImGuiRenderer::init_gpu_resources()
    {
      init_font_texture();
      init_shader();
      init_input_layout();
      init_root_signature();
      init_pso();
      init_imgui_renderstate();
    }

    void ImGuiRenderer::init_imgui_renderstate()
    {
      ImGuiResources resources{};

      resources.root_signature = m_root_signature.get();
      resources.pso = m_pipeline_state.get();
      imgui_init_resources(resources);
    }

    void ImGuiRenderer::init_main_imgui_viewport()
    {
      ImGuiViewport* main_viewport = ImGui::GetMainViewport();
      main_viewport->RendererUserData = debug_alloc<RexImGuiViewport>(main_viewport);
    }

    void ImGuiRenderer::init_font_texture()
    {
      // Build texture atlas
      ImGuiIO& io = ImGui::GetIO();
      unsigned char* pixels;
      s32 width, height;
      io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
      TextureFormat format = TextureFormat::Unorm4;

      m_fonts_texture = rhi::create_texture2d(width, height, format, pixels);

      ImGui::GetIO().Fonts->SetTexID((ImTextureID)m_fonts_texture.get());
    }
    void ImGuiRenderer::init_shader()
    {
      memory::Blob vertex_shader_content = vfs::read_file(path::join(vfs::engine_root(), "shaders", "imgui", rhi::shader_platform(), "imgui_vertex.hlsl"));
      memory::Blob pixel_shader_content = vfs::read_file(path::join(vfs::engine_root(), "shaders", "imgui", rhi::shader_platform(), "imgui_pixel.hlsl"));

      m_vertex_shader = rhi::create_vertex_shader(blob_to_string_view(vertex_shader_content));
      m_pixel_shader = rhi::create_pixel_shader(blob_to_string_view(pixel_shader_content));
    }
    void ImGuiRenderer::init_root_signature()
    {
      // The following should be auto generated from the shaders using reflection
      // That'd allow code to look like this
      //
      // RootSignatureDesc root_sig_desc;
      // root_sig_desc.vertex_shader = ..;
      // root_sig_desc.pixel_shader = ..;
      // ..
      // m_root_signature = create_root_signature(root_sig_desc);
      //
      // Which makes it much simpler to use

      RootSignatureDesc root_sig_desc{};
      // We have 2 constants for the shader, 1 in the vertex shader and 1 in the pixel shader
      root_sig_desc.views = rsl::make_unique<ShaderViewDesc[]>(1);
      root_sig_desc.views[0] = { "vertexBuffer", ShaderViewType::ConstantBufferView, 0, 0, ShaderVisibility::Vertex }; // We have 1 constant buffer in the vertex shader

      root_sig_desc.desc_tables = rsl::make_unique<DescriptorTableDesc[]>(1);
      root_sig_desc.desc_tables[0].ranges = rsl::make_unique<DescriptorRangeDesc[]>(1);
      root_sig_desc.desc_tables[0].ranges[0] = { DescriptorRangeType::ShaderResourceView, 1 }; // We have 1 src which points to our font texture
      root_sig_desc.desc_tables[0].visibility = ShaderVisibility::Pixel;

      // We have 1 sampler, used for sampling the font texture
      root_sig_desc.samplers = rsl::make_unique<ShaderSamplerDesc[]>(1);
      root_sig_desc.samplers[0].filtering = SamplerFiltering::MinMagMipLinear;
      root_sig_desc.samplers[0].address_mode_u = TextureAddressMode::Wrap;
      root_sig_desc.samplers[0].address_mode_v = TextureAddressMode::Wrap;
      root_sig_desc.samplers[0].address_mode_w = TextureAddressMode::Wrap;
      root_sig_desc.samplers[0].mip_lod_bias = 0.0f;
      root_sig_desc.samplers[0].max_anisotropy = 0;
      root_sig_desc.samplers[0].comparison_func = ComparisonFunc::Always;
      root_sig_desc.samplers[0].border_color = BorderColor::TransparentBlack;
      root_sig_desc.samplers[0].min_lod = 0.0f;
      root_sig_desc.samplers[0].max_lod = 0.0f;
      root_sig_desc.samplers[0].shader_register = 0;
      root_sig_desc.samplers[0].register_space = 0;
      root_sig_desc.samplers[0].shader_visibility = ShaderVisibility::Pixel;

      m_root_signature = rhi::create_root_signature(root_sig_desc);
    }
    void ImGuiRenderer::init_input_layout()
    {
      // The following should be auto genrated from the shaders using reflection
      // That'd allow code to look like this
      //
      // InputLayoutDesc input_layout_desc;
      // input_layout_desc.vertex_shader = ..;
      // ..
      // m_input_layout = create_input_layout(input_layout_desc);

      InputLayoutDesc input_layout_desc;
      input_layout_desc.input_layout =
      {
        InputLayoutElementDesc { "POSITION",  VertexBufferFormat::Float2, InputLayoutClassification::PerVertexData, 0, 0, 0, 0 },
        InputLayoutElementDesc { "TEXCOORD",  VertexBufferFormat::Float2, InputLayoutClassification::PerVertexData, 0, 0, 8, 0 },
        InputLayoutElementDesc { "COLOR", VertexBufferFormat::UNorm4, InputLayoutClassification::PerVertexData, 0, 0, 16, 0 }
      };
      m_input_layout = rhi::create_input_layout(input_layout_desc);
    }

    void ImGuiRenderer::init_pso()
    {
      // The following should be generated from a material.
      // The material gets loaded from disk, which then initializes the pso

       // Raster State
      RasterStateDesc rasterizer_desc{};
      rasterizer_desc.fill_mode = FillMode::Solid;
      rasterizer_desc.cull_mode = CullMode::None;
      rasterizer_desc.front_ccw = false;
      rasterizer_desc.depth_bias = 0;
      rasterizer_desc.depth_bias_clamp = 0.0f;
      rasterizer_desc.sloped_scale_depth_bias = 0.0f;
      rasterizer_desc.depth_clip_enable = true;
      rasterizer_desc.multisample_enable = false;
      rasterizer_desc.aa_lines_enable = false;
      rasterizer_desc.forced_sample_count = 0;
      m_raster_state = rsl::make_unique<RasterState>(rasterizer_desc);;

      PipelineStateDesc pso_desc{};
      pso_desc.input_layout = m_input_layout.get();
      pso_desc.raster_state = *m_raster_state.get();
      pso_desc.vertex_shader = m_vertex_shader.get();
      pso_desc.pixel_shader = m_pixel_shader.get();
      pso_desc.root_signature = m_root_signature.get();

      // Blend State
      pso_desc.blend_state = BlendDesc();
      BlendDesc& blend_state = pso_desc.blend_state.value();
      blend_state.enable_alpha_to_coverage = false;
      blend_state.render_target[0].blend_enable = true;
      blend_state.render_target[0].src_blend = Blend::SrcAlpha;
      blend_state.render_target[0].dst_blend = Blend::InvSrcAlpha;
      blend_state.render_target[0].blend_op = BlendOp::Add;
      blend_state.render_target[0].src_blend_alpha = Blend::One;
      blend_state.render_target[0].dst_blend_alpha = Blend::InvSrcAlpha;
      blend_state.render_target[0].blend_op_alpha = BlendOp::Add;
      blend_state.render_target[0].render_target_write_mask = RenderTargetWriteMask::All;

      // depth stencil state
      pso_desc.depth_stencil_state = DepthStencilDesc();
      DepthStencilDesc& depth_stencil_desc = pso_desc.depth_stencil_state.value();
      depth_stencil_desc.depth_enable = false;
      depth_stencil_desc.depth_write_mask = DepthWriteMask::DepthWriteMaskAll;
      depth_stencil_desc.depth_func = ComparisonFunc::Always;
      depth_stencil_desc.stencil_enable = false;
      depth_stencil_desc.front_face.stencil_fail_op = StencilOp::Keep;
      depth_stencil_desc.front_face.stencil_depth_fail_op = StencilOp::Keep;
      depth_stencil_desc.front_face.stencil_pass_op = StencilOp::Keep;
      depth_stencil_desc.front_face.stencil_func = ComparisonFunc::Always;
      depth_stencil_desc.back_face = depth_stencil_desc.front_face;

      m_pipeline_state = rhi::create_pso(pso_desc);
    }

    void ImGuiRenderer::destroy_viewports()
    {
      ImGuiViewport* main_viewport = ImGui::GetMainViewport();
      if (RexImGuiViewport* vd = (RexImGuiViewport*)main_viewport->RendererUserData)
      {
        rsl::destroy_at(vd);
        debug_dealloc(vd, sizeof(RexImGuiViewport));

        main_viewport->RendererUserData = nullptr;
      }

      ImGui::DestroyPlatformWindows();

      ImGuiIO& io = ImGui::GetIO();
      io.BackendRendererName = nullptr;
      io.BackendRendererUserData = nullptr;
    }


  }
}