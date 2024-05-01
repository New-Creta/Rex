#include "rex_directx/rendering/dx_imgui_renderer.h"

#include "imgui/platform/win/imgui_impl_win32.h"
#include <d3dcompiler.h>

#include "rex_directx/diagnostics/log.h"
#include "rex_directx/system/dx_rhi.h"

#include "rex_engine/memory/pointer_math.h"

#include "rex_renderer_core/rendering/viewport.h"

namespace rex::renderer::imgui
{
  DEFINE_LOG_CATEGORY(LogDxImGuiRenderer);

  struct VertexConstantBufferDx12
  {
    float   mvp[4][4];
  };

  ImGuiRenderer::ImGuiRenderer(HWND hwnd, DXGI_FORMAT rtvFormat, s32 maxFramesInFlight)
    : m_rtv_format(rtvFormat)
    , m_frame_idx(0)
    , m_font_texture_slot(rhi::ResourceSlot::make_invalid())
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

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
      style.WindowRounding = 0.0f;
      style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup platform/renderer backends
    ImGui_ImplWin32_Init(hwnd);
    
    io.BackendRendererUserData = this;
    io.BackendRendererName = "DirectX 12 ImGui Renderer";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

    m_frame_resources = rsl::make_unique<ImGuiFrameResource[]>(maxFramesInFlight);
    create_gfx_resources();
  }

  void ImGuiRenderer::setup_render_state(ImDrawData* drawData, ID3D12GraphicsCommandList* ctx, ImGuiFrameResource& fr)
  {
    // Setup orthographic projection matrix into our constant buffer
    // Our visible imgui space lies from drawData->DisplayPos (top left) to drawData->DisplayPos+data_data->DisplaySize (bottom right).
    VertexConstantBufferDx12 vertex_constant_buffer;
    {
      float L = drawData->DisplayPos.x;
      float R = drawData->DisplayPos.x + drawData->DisplaySize.x;
      float T = drawData->DisplayPos.y;
      float B = drawData->DisplayPos.y + drawData->DisplaySize.y;
      float mvp[4][4] =
      {
          { 2.0f / (R - L),   0.0f,           0.0f,       0.0f },
          { 0.0f,         2.0f / (T - B),     0.0f,       0.0f },
          { 0.0f,         0.0f,           0.5f,       0.0f },
          { (R + L) / (L - R),  (T + B) / (B - T),    0.5f,       1.0f },
      };
      memcpy(&vertex_constant_buffer.mvp, mvp, sizeof(mvp));
    }

    // Setup viewport
    Viewport viewport{};
    viewport.width = drawData->DisplaySize.x;
    viewport.height = drawData->DisplaySize.y;
    viewport.min_depth = 0.0f;
    viewport.max_depth = 1.0f;
    viewport.top_left_x = 0.0f;
    viewport.top_left_y = 0.0f;
    rhi::set_viewport(viewport);

    // Bind shader and vertex buffers
    rhi::set_vertex_buffer(fr.vertex_buffer_slot);
    rhi::set_index_buffer(fr.index_buffer_slot);
    rhi::set_primitive_topology(PrimitiveTopology::TriangleList);
    rhi::set_shader(m_shader_program);
    rhi::set_pso(m_pipeline_state);

    // Upload the constant buffer
    rhi::update_buffer(m_constant_buffer, &vertex_constant_buffer, sizeof(vertex_constant_buffer));
    rhi::set_constant_buffer(0, m_constant_buffer);

    // Setup blend factor
    const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
    ctx->OMSetBlendFactor(blend_factor);
  }

  void ImGuiRenderer::new_frame()
  {
    // Nothing to do here
    ImGui_ImplWin32_NewFrame();
  }

  void ImGuiRenderer::render_draw_data(ImDrawData* drawData, ID3D12GraphicsCommandList* ctx)
  {
    // Avoid rendering when minimized
    if (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f)
    {
      return;
    }

    // FIXME: I'm assuming that this only gets called once per frame!
    // If not, we can't just re-allocate the IB or VB, we'll have to do a proper allocator.
    ++m_frame_idx;
    if (m_frame_idx == m_frame_resources.count())
    {
      m_frame_idx = 0;
    }
    ImGuiFrameResource& fr = m_frame_resources[m_frame_idx];

    // Create and grow vertex/index buffers if needed
    if (!fr.vertex_buffer_slot.is_valid() || fr.VertexBufferSize < drawData->TotalVtxCount)
    {
      create_vertex_buffer(fr, drawData->TotalVtxCount);
    }
    if (!fr.index_buffer_slot.is_valid() || fr.IndexBufferSize < drawData->TotalIdxCount)
    {
      create_index_buffer(fr, drawData->TotalIdxCount);
    }

    for (int n = 0; n < drawData->CmdListsCount; n++)
    {
      const ImDrawList* cmd_list = drawData->CmdLists[n];
      rhi::update_buffer(fr.vertex_buffer_slot, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
      rhi::update_buffer(fr.index_buffer_slot, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
    }

    // Setup desired DX state
    setup_render_state(drawData, ctx, fr);

    // Render command lists
    // (Because we merged all buffers into a single one, we maintain our own offset into them)
    int global_vtx_offset = 0;
    int global_idx_offset = 0;
    ImVec2 clip_off = drawData->DisplayPos;
    for (int n = 0; n < drawData->CmdListsCount; n++)
    {
      const ImDrawList* cmd_list = drawData->CmdLists[n];
      for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
      {
        const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
        if (pcmd->UserCallback != nullptr)
        {
          // User callback, registered via ImDrawList::AddCallback()
          // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
          if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
            setup_render_state(drawData, ctx, fr);
          else
            pcmd->UserCallback(cmd_list, pcmd);
        }
        else
        {
          // Project scissor/clipping rectangles into framebuffer space
          ImVec2 clip_min(pcmd->ClipRect.x - clip_off.x, pcmd->ClipRect.y - clip_off.y);
          ImVec2 clip_max(pcmd->ClipRect.z - clip_off.x, pcmd->ClipRect.w - clip_off.y);
          if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
            continue;

          // Apply Scissor/clipping rectangle, Bind texture, Draw
          const D3D12_RECT r = { (LONG)clip_min.x, (LONG)clip_min.y, (LONG)clip_max.x, (LONG)clip_max.y };
          //D3D12_GPU_DESCRIPTOR_HANDLE texture_handle = {};
          //texture_handle.ptr = (UINT64)pcmd->GetTexID();
          //rhi::set_graphics_root_descriptor_table(rhi::ResourceID(pcmd->GetTexID()));
          auto texture = rhi::get_texture(m_font_texture_slot);
          auto x = texture->get()->AddRef();
          texture->get()->Release();
          ctx->SetGraphicsRootDescriptorTable(1, m_font_texture_srv.get_gpu());
          ctx->RSSetScissorRects(1, &r);
          ctx->DrawIndexedInstanced(pcmd->ElemCount, 1, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset, 0);
        }
      }
      global_idx_offset += cmd_list->IdxBuffer.Size;
      global_vtx_offset += cmd_list->VtxBuffer.Size;
    }
  }

  Error ImGuiRenderer::create_vertex_buffer(ImGuiFrameResource& fr, s32 size)
  {
    //fr.VertexBuffer.Reset();
    fr.VertexBufferSize = size + 5000;

    rhi::VertexBufferDesc desc{};
    desc.vertex_size = sizeof(ImDrawVert);
    fr.vertex_buffer_slot = rhi::create_vertex_buffer(fr.VertexBufferSize, sizeof(ImDrawVert));

    return Error::no_error();


    //D3D12_HEAP_PROPERTIES props;
    //memset(&props, 0, sizeof(D3D12_HEAP_PROPERTIES));
    //props.Type = D3D12_HEAP_TYPE_UPLOAD;
    //props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    //props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    //D3D12_RESOURCE_DESC desc;
    //memset(&desc, 0, sizeof(D3D12_RESOURCE_DESC));
    //desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    //desc.Width = fr.VertexBufferSize * sizeof(ImDrawVert);
    //desc.Height = 1;
    //desc.DepthOrArraySize = 1;
    //desc.MipLevels = 1;
    //desc.Format = DXGI_FORMAT_UNKNOWN;
    //desc.SampleDesc.Count = 1;
    //desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    //desc.Flags = D3D12_RESOURCE_FLAG_NONE;
    //if (m_device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&fr.VertexBuffer)) < 0)
    //{
    //  return;
    //}


  }

  Error ImGuiRenderer::create_index_buffer(ImGuiFrameResource& fr, s32 size)
  {
    IndexBufferFormat format = sizeof(ImDrawIdx) == 2
      ? IndexBufferFormat::Uint16
      : IndexBufferFormat::Uint32;

    fr.IndexBufferSize = size + 5000;
    fr.index_buffer_slot = rhi::create_index_buffer(fr.IndexBufferSize * sizeof(ImDrawIdx), format);

    return Error::no_error();

    //fr.IndexBuffer.Reset();
    //fr.IndexBufferSize = size + 10000;
    //D3D12_HEAP_PROPERTIES props;
    //memset(&props, 0, sizeof(D3D12_HEAP_PROPERTIES));
    //props.Type = D3D12_HEAP_TYPE_UPLOAD;
    //props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    //props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    //D3D12_RESOURCE_DESC desc;
    //memset(&desc, 0, sizeof(D3D12_RESOURCE_DESC));
    //desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    //desc.Width = fr.IndexBufferSize * sizeof(ImDrawIdx);
    //desc.Height = 1;
    //desc.DepthOrArraySize = 1;
    //desc.MipLevels = 1;
    //desc.Format = DXGI_FORMAT_UNKNOWN;
    //desc.SampleDesc.Count = 1;
    //desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    //desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    //if (m_device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&fr.IndexBuffer)) < 0)
    //{
    //  return;
    //}


  }

  Error ImGuiRenderer::create_constant_buffer()
  {
    m_constant_buffer = rhi::create_constant_buffer(sizeof(VertexConstantBufferDx12));
    return Error::no_error();
  }

  Error ImGuiRenderer::create_shader()
  {
    // Compile the shaders
    static rsl::string_view vertex_shader =
      "cbuffer vertexBuffer : register(b0) \
      {\
        float4x4 ProjectionMatrix; \
      };\
      struct VS_INPUT\
      {\
        float2 pos : POSITION;\
        float4 col : COLOR0;\
        float2 uv  : TEXCOORD0;\
      };\
      \
      struct PS_INPUT\
      {\
        float4 pos : SV_POSITION;\
        float4 col : COLOR0;\
        float2 uv  : TEXCOORD0;\
      };\
      \
      PS_INPUT main(VS_INPUT input)\
      {\
        PS_INPUT output;\
        output.pos = mul( ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));\
        output.col = input.col;\
        output.uv  = input.uv;\
        return output;\
      }";

    rhi::CompileShaderDesc compile_vs_desc{};
    compile_vs_desc.shader_code = rex::memory::Blob(rsl::make_unique<char[]>(vertex_shader.length()));
    compile_vs_desc.shader_code.write(vertex_shader.data(), vertex_shader.length());
    compile_vs_desc.shader_entry_point = "main";
    compile_vs_desc.shader_feature_target = "vs_5_0";
    compile_vs_desc.shader_name = "imgui_vertex_shader";
    compile_vs_desc.shader_type = rhi::ShaderType::Vertex;
    m_vertex_shader = rhi::compile_shader(compile_vs_desc);

    static rsl::string_view pixel_shader =
      "struct PS_INPUT\
       {\
         float4 pos : SV_POSITION;\
         float4 col : COLOR0;\
         float2 uv  : TEXCOORD0;\
       };\
       SamplerState sampler0 : register(s0);\
       Texture2D texture0 : register(t0);\
       \
       float4 main(PS_INPUT input) : SV_Target\
       {\
         float4 out_col = input.col * texture0.Sample(sampler0, input.uv); \
         return out_col; \
       }";

    rhi::CompileShaderDesc compile_ps_desc{};
    compile_ps_desc.shader_code = rex::memory::Blob(rsl::make_unique<char[]>(pixel_shader.length()));
    compile_ps_desc.shader_code.write(pixel_shader.data(), pixel_shader.length());
    compile_ps_desc.shader_entry_point = "main";
    compile_ps_desc.shader_feature_target = "ps_5_0";
    compile_ps_desc.shader_name = "imgui_pixel_shader";
    compile_ps_desc.shader_type = rhi::ShaderType::Pixel;
    m_pixel_shader = rhi::compile_shader(compile_ps_desc);

    // Link shaders
    rhi::LinkShaderDesc link_shader_desc{};
    link_shader_desc.vertex_shader = m_vertex_shader;
    link_shader_desc.pixel_shader = m_pixel_shader;

    // We have 2 constants for the shader, 1 in the vertex shader and 1 in the pixel shader
    link_shader_desc.constants = rsl::make_unique<rex::rhi::ShaderParameterLayoutDescription[]>(1);
    link_shader_desc.constants[0] = { rex::rhi::ShaderParameterType::CBuffer, "vertexBuffer", 0, renderer::ShaderVisibility::Vertex }; // We have 1 constant buffer in the vertex shader

    link_shader_desc.desc_tables = rsl::make_unique<rex::rhi::DescriptorTableDescription[]>(1);
    link_shader_desc.desc_tables[0].ranges = rsl::make_unique<rex::rhi::DescriptorRange[]>(1);
    link_shader_desc.desc_tables[0].ranges[0] = { rex::rhi::DescriptorRangeType::ShaderResourceView, 1 }; // We have 1 src which points to our font texture
    link_shader_desc.desc_tables[0].visibility = ShaderVisibility::Pixel;

    // We have 1 sampler, used for sampling the font texture
    link_shader_desc.samplers = rsl::make_unique<rhi::ShaderSamplerDescription[]>(1);
    link_shader_desc.samplers[0].filtering = renderer::SamplerFiltering::MinMagMipLinear;
    link_shader_desc.samplers[0].address_mode_u = renderer::TextureAddressMode::Wrap;
    link_shader_desc.samplers[0].address_mode_v = renderer::TextureAddressMode::Wrap;
    link_shader_desc.samplers[0].address_mode_w = renderer::TextureAddressMode::Wrap;
    link_shader_desc.samplers[0].mip_lod_bias = 0.0f;
    link_shader_desc.samplers[0].max_anisotropy = 0;
    link_shader_desc.samplers[0].comparison_func = renderer::ComparisonFunc::Always;
    link_shader_desc.samplers[0].border_color = renderer::BorderColor::TransparentBlack;
    link_shader_desc.samplers[0].min_lod = 0.0f;
    link_shader_desc.samplers[0].max_lod = 0.0f;
    link_shader_desc.samplers[0].shader_register = 0;
    link_shader_desc.samplers[0].register_space = 0;
    link_shader_desc.samplers[0].shader_visibility = renderer::ShaderVisibility::Pixel;

    m_shader_program = rex::rhi::link_shader(link_shader_desc);

    return Error::no_error();
  }

  Error ImGuiRenderer::create_pso()
  {
    // Input Layout
    rex::rhi::InputLayoutDesc input_layout_desc;
    input_layout_desc.input_layout =
    {
      rex::rhi::InputLayoutElementDesc { "POSITION",  rex::renderer::VertexBufferFormat::Float2, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 0, 0 },
      rex::rhi::InputLayoutElementDesc { "TEXCOORD",  rex::renderer::VertexBufferFormat::Float2, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 8, 0 },
      rex::rhi::InputLayoutElementDesc { "COLOR", rex::renderer::VertexBufferFormat::UNorm4, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 16, 0 }
    };
    m_input_layout = rhi::create_input_layout(input_layout_desc);

    // Raster State
    rhi::RasterStateDesc rasterizer_desc{};
    rasterizer_desc.fill_mode = renderer::FillMode::Solid;
    rasterizer_desc.cull_mode = renderer::CullMode::None;
    rasterizer_desc.front_ccw = false;
    rasterizer_desc.depth_bias = D3D12_DEFAULT_DEPTH_BIAS;
    rasterizer_desc.depth_bias_clamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    rasterizer_desc.sloped_scale_depth_bias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    rasterizer_desc.depth_clip_enable = true;
    rasterizer_desc.multisample = false;
    rasterizer_desc.aa_lines = false;
    rasterizer_desc.forced_sample_count = 0;

    rhi::PipelineStateDesc pso_desc{};
    pso_desc.input_layout = m_input_layout;
    pso_desc.raster_state = rhi::create_raster_state(rasterizer_desc);
    pso_desc.shader = m_shader_program;

    // Blend State
    pso_desc.blend_state = rhi::BlendState();
    rhi::BlendState& blend_state = pso_desc.blend_state.value();
    blend_state.enable_alpha_to_coverage = false;
    blend_state.render_target[0].blend_enable = true;
    blend_state.render_target[0].src_blend = rhi::Blend::SrcAlpha;
    blend_state.render_target[0].dst_blend = rhi::Blend::InvSrcAlpha;
    blend_state.render_target[0].blend_op = rhi::BlendOp::Add;
    blend_state.render_target[0].src_blend_alpha = rhi::Blend::One;
    blend_state.render_target[0].dst_blend_alpha = rhi::Blend::InvSrcAlpha;
    blend_state.render_target[0].blend_op_alpha = rhi::BlendOp::Add;
    blend_state.render_target[0].render_target_write_mask = rhi::RenderTargetWriteMask::All;
    
    // depth stencil state
    pso_desc.depth_stencil_state = rhi::DepthStencilDesc();
    rhi::DepthStencilDesc& depth_stencil_desc = pso_desc.depth_stencil_state.value();
    depth_stencil_desc.depth_enable = false;
    depth_stencil_desc.depth_write_mask = rhi::DepthWriteMask::DepthWriteMaskAll;
    depth_stencil_desc.depth_func = renderer::ComparisonFunc::Always;
    depth_stencil_desc.stencil_enable = false;
    depth_stencil_desc.front_face.stencil_fail_op = rhi::StencilOp::Keep;
    depth_stencil_desc.front_face.stencil_depth_fail_op = rhi::StencilOp::Keep;
    depth_stencil_desc.front_face.stencil_pass_op = rhi::StencilOp::Keep;
    depth_stencil_desc.front_face.stencil_func = renderer::ComparisonFunc::Always;
    depth_stencil_desc.back_face = depth_stencil_desc.front_face;

    m_pipeline_state = rhi::create_pso(pso_desc);

    return Error::no_error();
  }

  Error ImGuiRenderer::create_gfx_resources()
  {
    Error err = Error::no_error();

    err = create_shader();
    if (err)
    {
      return err;
    }

    err = create_constant_buffer();
    if (err)
    {
      return err;
    }

    err = create_pso();
    if (err)
    {
      return err;
    }
   
    err = create_fonts_texture();
    if (err)
    {
      return err;
    }

    return err;
  }

  Error ImGuiRenderer::create_fonts_texture()
  {
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    s32 width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
    m_font_texture_slot = rhi::create_texture2d((const char*)pixels, format, width, height);

    // Create texture view
    m_font_texture_srv = rhi::create_texture2d_srv(m_font_texture_slot);

    // Store our identifier
    // READ THIS IF THE STATIC_ASSERT() TRIGGERS:
    // - Important: to compile on 32-bit systems, this backend requires code to be compiled with '#define ImTextureID ImU64'.
    // - This is because we need ImTextureID to carry a 64-bit value and by default ImTextureID is defined as void*.
    // [Solution 1] IDE/msbuild: in "Properties/C++/Preprocessor Definitions" add 'ImTextureID=ImU64' (this is what we do in the 'example_win32_direct12/example_win32_direct12.vcxproj' project file)
    // [Solution 2] IDE/msbuild: in "Properties/C++/Preprocessor Definitions" add 'IMGUI_USER_CONFIG="my_imgui_config.h"' and inside 'my_imgui_config.h' add '#define ImTextureID ImU64' and as many other options as you like.
    // [Solution 3] IDE/msbuild: edit imconfig.h and add '#define ImTextureID ImU64' (prefer solution 2 to create your own config file!)
    // [Solution 4] command-line: add '/D ImTextureID=ImU64' to your cl.exe command-line (this is what we do in the example_win32_direct12/build_win32.bat file)
    static_assert(sizeof(ImTextureID) >= sizeof(m_font_texture_slot.slot_id()), "Can't pack descriptor handle into TexID, 32-bit not supported yet.");
    io.Fonts->SetTexID(reinterpret_cast<ImTextureID>(static_cast<u64>(m_font_texture_slot.slot_id())));

    return Error::no_error();
  }
}