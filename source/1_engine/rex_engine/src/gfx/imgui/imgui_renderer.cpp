#include "rex_engine/gfx/imgui/imgui_renderer.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/memory/global_allocator.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"

#include "rex_engine/gfx/system/rhi.h"
#include "rex_engine/gfx/resources/texture_2d.h"
#include "rex_engine/gfx/core/texture_format.h"


#include "rex_engine/gfx/imgui/imgui_frame_context.h"
#include "rex_engine/gfx/imgui/imgui_viewport.h"
#include "rex_engine/gfx/imgui/imgui_window_render_params.h"

#include "rex_engine/gfx/system/shader_library.h"

#include "rex_engine/gfx/graphics.h"

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

    // Setup imgui to accept new render commands
    void ImGuiRenderer::new_frame()
    {
      imgui_platform_new_frame();

      ImGui::NewFrame();
    }

    // Render a single frame of imgui widgets
    void ImGuiRenderer::render()
    {
      ImGui::Render();

      auto render_ctx = gfx::new_render_ctx();

      // As the imgui renderer main viewport is the main window of the application
      // we use the main window as render target, which is the default render target
      // We don't have to set this manually as it gets set when a context gets reset

      ImGuiViewport* main_viewport = ImGui::GetMainViewport();
      if (RexImGuiViewport* rex_viewport = (RexImGuiViewport*)main_viewport->RendererUserData)
      {
        m_imgui_renderpass->bind_to(render_ctx.get());
        rex_viewport->render(render_ctx.get(), m_imgui_renderpass.get());
      }

      // Update and render the imgui windows if any
      if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
      {
        ImGuiWindowRenderParams render_params{};
        render_params.render_pass = m_imgui_renderpass.get();

        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault(nullptr, &render_params);
      }

      // Open imgui to accept new commands again
      new_frame();
    }

    // Initialize ImGui library
    void ImGuiRenderer::init_imgui(void* platformWindowHandle)
    {
      IMGUI_CHECKVERSION();
      ImGui::CreateContext();
      ImGuiIO& io = ImGui::GetIO();
      io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard controls
      io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable gamepad controls
      io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable docking
      io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable multi-viewport / Platform windows
      io.ConfigFlags |= ImGuiConfigFlags_IsSRGB; // We're rendering in sRGBs

      // Enable dark mode
      ImGui::StyleColorsDarkSRGB();

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

    // Initialize all gpu resources needed for imgui rendering
    void ImGuiRenderer::init_gpu_resources()
    {
      init_font_texture();
      init_font_sampler();
      init_renderpass();
    }

    // Initialize the main imgui viewport, which is pointing to the application's main window
    void ImGuiRenderer::init_main_imgui_viewport()
    {
      ImGuiViewport* main_viewport = ImGui::GetMainViewport();
      main_viewport->RendererUserData = debug_alloc<RexImGuiViewport>(main_viewport);
    }

    // Load the fonts texture from ImGui backend and create a texture resource on the GPU for it
    void ImGuiRenderer::init_font_texture()
    {
      // Build texture atlas, by default this sits in memory in imgui
      unsigned char* pixels;
      s32 width, height;
      ImGuiIO& io = ImGui::GetIO();
      io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
      TextureFormat format = TextureFormat::Unorm4;

      m_fonts_texture = rhi::create_texture2d(width, height, format, pixels);

      io.Fonts->SetTexID((ImTextureID)m_fonts_texture.get());
    }
    // Create the sampler, used by ImGui
    void ImGuiRenderer::init_font_sampler()
    {
      // Sampler is currently hardcoded
      SamplerDesc desc{};
      desc.filtering = SamplerFiltering::MinMagMipLinear;
      desc.address_mode_u = TextureAddressMode::Wrap;
      desc.address_mode_v = TextureAddressMode::Wrap;
      desc.address_mode_w = TextureAddressMode::Wrap;
      desc.mip_lod_bias = 0.0f;
      desc.max_anisotropy = 0;
      desc.comparison_func = ComparisonFunc::Always;
      desc.border_color = BorderColor::TransparentBlack;
      desc.min_lod = 0.0f;
      desc.max_lod = 0.0f;
      desc.shader_register = 0;
      desc.register_space = 0;
      desc.shader_visibility = ShaderVisibility::Pixel;

      m_fonts_sampler = rhi::create_sampler2d(desc);
    }
    // Initialize the material that'll be used by all ImGui rendering
    void ImGuiRenderer::init_renderpass()
    {
      RenderPassDesc imgui_pass_desc{};

      imgui_pass_desc.name = "ImGui Pass";

      // blend state
      BlendDesc& blend_state = imgui_pass_desc.pso_desc.output_merger.blend_state;
      blend_state.enable_alpha_to_coverage = false;
      blend_state.independent_blend_state = false;
      blend_state.render_target[0].blend_enable = true;
      blend_state.render_target[0].src_blend = Blend::SrcAlpha;
      blend_state.render_target[0].dst_blend = Blend::InvSrcAlpha;
      blend_state.render_target[0].blend_op = BlendOp::Add;
      blend_state.render_target[0].src_blend_alpha = Blend::One;
      blend_state.render_target[0].dst_blend_alpha = Blend::InvSrcAlpha;
      blend_state.render_target[0].blend_op_alpha = BlendOp::Add;
      blend_state.render_target[0].render_target_write_mask = RenderTargetWriteMask::All;

      // raster state
      RasterStateDesc& raster_state = imgui_pass_desc.pso_desc.output_merger.raster_state;
      raster_state.fill_mode = FillMode::Solid;
      raster_state.cull_mode = CullMode::None;
      raster_state.front_ccw = false;
      raster_state.depth_bias = 0;
      raster_state.depth_bias_clamp = 0.0f;
      raster_state.sloped_scale_depth_bias = 0.0f;
      raster_state.depth_clip_enable = true;
      raster_state.multisample_enable = false;
      raster_state.aa_lines_enable = false;
      raster_state.forced_sample_count = 0;

      // depth stencil
      DepthStencilDesc& depth_stencil_state = imgui_pass_desc.pso_desc.output_merger.depth_stencil_state;
      depth_stencil_state.depth_enable = false;
      depth_stencil_state.depth_write_mask = DepthWriteMask::DepthWriteMaskAll;
      depth_stencil_state.depth_func = ComparisonFunc::Always;
      depth_stencil_state.stencil_enable = false;
      depth_stencil_state.front_face.stencil_fail_op = StencilOp::Keep;
      depth_stencil_state.front_face.stencil_depth_fail_op = StencilOp::Keep;
      depth_stencil_state.front_face.stencil_pass_op = StencilOp::Keep;
      depth_stencil_state.front_face.stencil_func = ComparisonFunc::Less;
      depth_stencil_state.back_face = depth_stencil_state.front_face;

      // frame buffers
      imgui_pass_desc.framebuffer_desc.emplace_back(swapchain_frame_buffer_handle());

      imgui_pass_desc.pso_desc.shader_pipeline.vs = shader_lib::load(path::join(vfs::mount_path(MountingPoint::EngineShaders), "imgui", "hlsl", "imgui_vertex.hlsl"), ShaderType::Vertex);
      imgui_pass_desc.pso_desc.shader_pipeline.ps = shader_lib::load(path::join(vfs::mount_path(MountingPoint::EngineShaders), "imgui", "hlsl", "imgui_pixel.hlsl"), ShaderType::Pixel);

      imgui_pass_desc.pso_desc.input_layout = 
      {
        InputLayoutElementDesc{ ShaderSemantic::Position,  VertexBufferFormat::Float2},
        InputLayoutElementDesc{ ShaderSemantic::TexCoord,  VertexBufferFormat::Float2},
        InputLayoutElementDesc{ ShaderSemantic::Color, VertexBufferFormat::UChar4Norm}
      };


      m_imgui_renderpass = rsl::make_unique<RenderPass>(imgui_pass_desc);
      m_imgui_renderpass->set("fonts_texture", m_fonts_texture.get());
      m_imgui_renderpass->set("fonts_sampler", m_fonts_sampler.get());
      m_imgui_renderpass->set_blend_factor({ 0.0f, 0.0f, 0.0f, 0.0f });
    }

    // Destroy all viewports used by ImGui
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