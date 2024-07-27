#include "rex_renderer_core/imgui/imgui_renderer.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/memory/global_allocator.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"

#include "rex_renderer_core/gfx/rhi.h"
#include "rex_renderer_core/resources/texture_2d.h"
#include "rex_renderer_core/gfx/texture_format.h"
#include "rex_renderer_core/system/shader_elements.h"

#include "rex_renderer_core/imgui/imgui_frame_context.h"
#include "rex_renderer_core/imgui/imgui_viewport.h"
#include "rex_renderer_core/imgui/imgui_window_render_params.h"

#include "rex_renderer_core/materials/material_system.h"

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

      auto render_ctx = gfx::new_render_ctx(m_pipeline_state.get());

      // As the imgui renderer main viewport is the main window of the application
      // we use the main window as render target, which is the default render target
      // We don't have to set this manually as it gets set when a context gets reset

      ImGuiViewport* main_viewport = ImGui::GetMainViewport();
      if (RexImGuiViewport* rex_viewport = (RexImGuiViewport*)main_viewport->RendererUserData)
      {
        render_ctx->bind_material(m_material.get());
        rex_viewport->render(*render_ctx);
      }

      // Update and render the imgui windows if any
      if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
      {
        ImGuiWindowRenderParams render_params{};
        render_params.pso = m_pipeline_state.get();
        render_params.material = m_material.get();

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

    // Initialize all gpu resources needed for imgui rendering
    void ImGuiRenderer::init_gpu_resources()
    {
      init_font_texture();
      init_font_sampler();
      init_input_layout();
      init_material();
      init_pso();
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
      ShaderSamplerDesc desc{};
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
    // Initialize the input layout that'll be used by all ImGui rendering
    void ImGuiRenderer::init_input_layout()
    {
      // Input layout is hardcoded as the vertices are also hardcoded
      // The material is responsible for validating that the input layout is correct with the shader

      //InputLayoutDesc input_layout_desc;
      //input_layout_desc.input_layout =
      //{
      //  InputLayoutElementDesc { ShaderSemantic::Position,  VertexBufferFormat::Float2, InputLayoutClassification::PerVertex, 0, 0, 0, 0 },
      //  InputLayoutElementDesc { ShaderSemantic::TexCoord,  VertexBufferFormat::Float2, InputLayoutClassification::PerVertex, 0, 0, 8, 0 },
      //  InputLayoutElementDesc { ShaderSemantic::Color, VertexBufferFormat::UChar4Norm, InputLayoutClassification::PerVertex, 0, 0, 16, 0 }
      //};
      //m_input_layout = rhi::create_input_layout(rsl::move(input_layout_desc));
    }
    // Initialize the material that'll be used by all ImGui rendering
    void ImGuiRenderer::init_material()
    {
      // Load the material from disk and initialize it with the parameters loaded at runtime
      rsl::string material_path = path::join(vfs::engine_root(), "materials", "imgui.material");
      m_material = load_material(material_path);
      REX_ASSERT_X(m_material != nullptr, "Failed to load imgui material");

      m_material->set_texture("fonts_texture", m_fonts_texture.get());
      m_material->set_sampler("fonts_sampler", m_fonts_sampler.get());
      m_material->set_blend_factor({ 0.0f, 0.0f, 0.0f, 0.0f });

      //m_material->validate_input_layout(m_input_layout.get());
    }
    // Initialize the pso based on the the gpu resources
    void ImGuiRenderer::init_pso()
    {
      InputLayoutDesc input_layout_desc =
      {
        InputLayoutElementDesc { ShaderSemantic::Position,  VertexBufferFormat::Float2, InputLayoutClassification::PerVertex, 0, 0, 0, 0 },
        InputLayoutElementDesc { ShaderSemantic::TexCoord,  VertexBufferFormat::Float2, InputLayoutClassification::PerVertex, 0, 0, 8, 0 },
        InputLayoutElementDesc { ShaderSemantic::Color, VertexBufferFormat::UChar4Norm, InputLayoutClassification::PerVertex, 0, 0, 16, 0 }
      };
      m_pipeline_state = rhi::create_pso(input_layout_desc, m_material.get());
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