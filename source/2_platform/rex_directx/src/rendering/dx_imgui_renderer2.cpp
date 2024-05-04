#include "imgui/imgui.h"
#ifndef IMGUI_DISABLE
#include "rex_directx/rendering/dx_imgui_renderer2.h"

#include "imgui/platform/win/imgui_impl_win32.h"

// DirectX
#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#ifdef _MSC_VER
#pragma comment(lib, "d3dcompiler") // Automatically link with d3dcompiler.lib as we are using D3DCompile() below.
#endif

#include "rex_directx/system/dx_commandlist.h"
#include "rex_directx/system/dx_commandlist2.h"
#include "rex_directx/system/dx_command_queue.h"
#include "rex_directx/system/dx_constant_buffer.h"
#include "rex_directx/system/dx_debug_interface.h"
#include "rex_directx/system/dx_descriptor_heap.h"
#include "rex_directx/system/dx_device.h"
#include "rex_directx/system/dx_feature_level.h"
#include "rex_directx/system/dx_feature_shader_model.h"
#include "rex_directx/system/dx_fence.h"
#include "rex_directx/system/dx_index_buffer.h"
#include "rex_directx/system/dx_pipeline_library.h"
#include "rex_directx/system/dx_pipeline_state.h"
#include "rex_directx/system/dx_resource.h"
#include "rex_directx/system/dx_resource_heap.h"
#include "rex_directx/system/dx_rhi.h"
#include "rex_directx/system/dx_shader_compiler.h"
#include "rex_directx/system/dx_swapchain.h"
#include "rex_directx/system/dx_texture_2d.h"
#include "rex_directx/system/dx_vertex_buffer.h"
#include "rex_directx/resources/dx_shader_program_resource.h"

#include "rex_directx/diagnostics/log.h"

// DirectX data

// These resources are shared between all viewports
// They're initialized at startup and destroyed on shutdown
// They're one of the "set once, use forever" resources
struct ImGui_ImplDX12_Data
{
  ID3D12Device1* pd3dDevice;                             // Needed to initialize child windows with their own directx objects
  DXGI_FORMAT                 RTVFormat;                 // Comes in from the main rendered, to match child windows rtv format with that of the main window
  rex::rhi::DescriptorHeap* pd3dSrvDescHeap;             // Probably not needed, rex uses single descriptor heaps throughout the process.
  rex::rhi::DescriptorHandle texture_handle;             // the handle to the shader resource view of the font texture
  UINT                        numFramesInFlight;         // used to store the number of frames we can handle at once.

  rex::rhi::ResourceSlot shader_program;                 // resource slot for the compiled imgui shader as well as its root signature
  rex::rhi::ResourceSlot pipeline_state;                 // resource slot for the pipeline state of imgui
  rex::rhi::ResourceSlot constant_buffer;                // resource slot for the constant buffer holding the mvp matrix. This single object is shared between all viewports
  rex::rhi::ResourceSlot input_layout;                   // resource slot for the input layout of imgui
  rex::rhi::ResourceSlot vertex_shader;                  // resource slot for the vertex shader of imgui
  rex::rhi::ResourceSlot pixel_shader;                   // resource slot for the pixel shader of imgui
  rex::rhi::ResourceSlot texture;                        // resources lot for the fonts texture, used by imgui

  ImGui_ImplDX12_Data() { memset((void*)this, 0, sizeof(*this)); }
};

// Backend data stored in io.BackendRendererUserData to allow support for multiple Dear ImGui contexts
// It is STRONGLY preferred that you use docking branch with multi-viewports (== single Dear ImGui context + multiple windows) instead of multiple Dear ImGui contexts.
static ImGui_ImplDX12_Data* ImGui_ImplDX12_GetBackendData()
{
  return ImGui::GetCurrentContext() ? (ImGui_ImplDX12_Data*)ImGui::GetIO().BackendRendererUserData : nullptr;
}

// Buffers used during the rendering of a frame
struct ImGui_ImplDX12_RenderBuffers
{
  int                 IndexBufferSize;    // the number of indices the index buffer supports
  int                 VertexBufferSize;   // the number of vertices the vertex buffer supports

  rex::rhi::ResourceSlot vertex_buffer;   // resource slot of the vertex buffer
  rex::rhi::ResourceSlot index_buffer;    // resource slot of the index buffer
};

// Buffers used for secondary viewports created by the multi-viewports systems
// We support multiple frames in flight, so we need to have a few resources that we need per frame
// which cannot be shared between frames
struct ImGui_ImplDX12_FrameContext
{
  ID3D12Resource* RenderTarget;               
  D3D12_CPU_DESCRIPTOR_HANDLE     RenderTargetCpuDescriptors;

  rsl::unique_ptr<rex::rhi::CommandAllocator> command_allocator;

};

// Helper structure we store in the void* RendererUserData field of each ImGuiViewport to easily retrieve our backend data.
// Main viewport created by application will only use the Resources field.
// Secondary viewports created by this backend will use all the fields (including Window fields),

// Each viewport holds some of its own resources which aren't shared by other viewports
// the below is to manage each on of these
struct ImGui_ImplDX12_ViewportData
{
  // Window
  rsl::unique_ptr<rex::rhi::CommandList2> command_list;
  UINT64                          FenceSignaledValue;
  UINT                            NumFramesInFlight;
  ImGui_ImplDX12_FrameContext* FrameCtx;

  // Render buffers
  UINT                            FrameIndex;
  ImGui_ImplDX12_RenderBuffers* FrameRenderBuffers;

  rsl::unique_ptr<rex::rhi::CommandQueue> rex_command_queue;
  rsl::unique_ptr<rex::rhi::DescriptorHeap> rex_descriptor_heap;
  rsl::unique_ptr<rex::rhi::Swapchain> rex_swapchain;

  ImGui_ImplDX12_ViewportData(UINT num_frames_in_flight)
  {
    FenceSignaledValue = 0;
    NumFramesInFlight = num_frames_in_flight;
    FrameCtx = new ImGui_ImplDX12_FrameContext[NumFramesInFlight];
    FrameIndex = UINT_MAX;
    FrameRenderBuffers = new ImGui_ImplDX12_RenderBuffers[NumFramesInFlight];

    for (UINT i = 0; i < NumFramesInFlight; ++i)
    {
      FrameCtx[i].RenderTarget = nullptr;

      // Create buffers with a default size (they will later be grown as needed)
      FrameRenderBuffers[i].VertexBufferSize = 5000;
      FrameRenderBuffers[i].IndexBufferSize = 10000;
    }
  }
  ~ImGui_ImplDX12_ViewportData()
  {
    //IM_ASSERT(CommandList == nullptr);

    for (UINT i = 0; i < NumFramesInFlight; ++i)
    {
      IM_ASSERT(/*FrameCtx[i].CommandAllocator == nullptr && */FrameCtx[i].RenderTarget == nullptr);
    }

    delete[] FrameCtx; FrameCtx = nullptr;
    delete[] FrameRenderBuffers; FrameRenderBuffers = nullptr;
  }
};

struct VERTEX_CONSTANT_BUFFER_DX12
{
  float   mvp[4][4];
};

// First render the main window widgets
// Next render all the child windows

// Forward Declarations
static void ImGui_ImplDX12_InitPlatformInterface();
static void ImGui_ImplDX12_ShutdownPlatformInterface();

// Functions
static void ImGui_ImplDX12_SetupRenderState(ImDrawData* draw_data, ID3D12GraphicsCommandList* ctx, ImGui_ImplDX12_RenderBuffers* fr)
{
  ImGui_ImplDX12_Data* bd = ImGui_ImplDX12_GetBackendData();

  // Setup orthographic projection matrix into our constant buffer
  // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right).
  VERTEX_CONSTANT_BUFFER_DX12 vertex_constant_buffer;
  {
    float L = draw_data->DisplayPos.x;
    float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
    float T = draw_data->DisplayPos.y;
    float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
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
  D3D12_VIEWPORT vp;
  memset(&vp, 0, sizeof(D3D12_VIEWPORT));
  vp.Width = draw_data->DisplaySize.x;
  vp.Height = draw_data->DisplaySize.y;
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;
  vp.TopLeftX = vp.TopLeftY = 0.0f;
  ctx->RSSetViewports(1, &vp);

  rex::rhi::set_vertex_buffer(fr->vertex_buffer, ctx);
  rex::rhi::set_index_buffer(fr->index_buffer, ctx);

  ctx->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  ctx->SetPipelineState(rex::rhi::get_pso(bd->pipeline_state)->get());
  ctx->SetGraphicsRootSignature(rex::rhi::get_shader(bd->shader_program)->root_signature());

  rex::rhi::update_buffer(bd->constant_buffer, &vertex_constant_buffer, sizeof(vertex_constant_buffer), ctx);
  rex::rhi::set_constant_buffer(0, bd->constant_buffer, ctx);

  // Setup blend factor
  const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
  ctx->OMSetBlendFactor(blend_factor);
}

template<typename T>
static inline void SafeRelease(T*& res)
{
  if (res)
    res->Release();
  res = nullptr;
}

// Render function
void ImGui_ImplDX12_RenderDrawData(ImDrawData* draw_data, ID3D12GraphicsCommandList* ctx)
{
  // Avoid rendering when minimized
  if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
    return;

  if (draw_data->CmdListsCount == 0)
    return;

  ImGui_ImplDX12_Data* bd = ImGui_ImplDX12_GetBackendData();
  ImGui_ImplDX12_ViewportData* vd = (ImGui_ImplDX12_ViewportData*)draw_data->OwnerViewport->RendererUserData;
  vd->FrameIndex++;
  ImGui_ImplDX12_RenderBuffers* fr = &vd->FrameRenderBuffers[vd->FrameIndex % bd->numFramesInFlight];

  // Create and grow vertex/index buffers if needed
  if (!fr->vertex_buffer.is_valid() || fr->VertexBufferSize < draw_data->TotalVtxCount)
  {
    fr->VertexBufferSize = draw_data->TotalVtxCount + 5000;
    fr->vertex_buffer = rex::rhi::create_vertex_buffer(sizeof(ImDrawVert) * fr->VertexBufferSize, sizeof(ImDrawVert));
  }
  if (!fr->index_buffer.is_valid() || fr->IndexBufferSize < draw_data->TotalIdxCount)
  {
    rex::renderer::IndexBufferFormat format = sizeof(ImDrawIdx) == 2
      ? rex::renderer::IndexBufferFormat::Uint16
      : rex::renderer::IndexBufferFormat::Uint32;
    fr->IndexBufferSize = draw_data->TotalIdxCount + 10000;
    fr->index_buffer = rex::rhi::create_index_buffer(fr->IndexBufferSize * sizeof(ImDrawIdx), format);
  }

  // Upload vertex/index data into a single contiguous GPU buffer
  s32 vtx_offset = 0;
  s32 idx_offset = 0;
  
  for (int n = 0; n < draw_data->CmdListsCount; n++)
  {
    const ImDrawList* cmd_list = draw_data->CmdLists[n];
    rex::rhi::update_buffer(fr->vertex_buffer, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), ctx, vtx_offset);
    rex::rhi::update_buffer(fr->index_buffer, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), ctx, idx_offset);
    vtx_offset += cmd_list->VtxBuffer.Size * sizeof(ImDrawVert);
    idx_offset += cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);
  }

  // Setup desired DX state
  ImGui_ImplDX12_SetupRenderState(draw_data, ctx, fr);

  // Render command lists
  // (Because we merged all buffers into a single one, we maintain our own offset into them)
  int global_vtx_offset = 0;
  int global_idx_offset = 0;
  ImVec2 clip_off = draw_data->DisplayPos;
  for (int n = 0; n < draw_data->CmdListsCount; n++)
  {
    const ImDrawList* cmd_list = draw_data->CmdLists[n];
    for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
    {
      const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
      if (pcmd->UserCallback != nullptr)
      {
        // User callback, registered via ImDrawList::AddCallback()
        // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
        if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
          ImGui_ImplDX12_SetupRenderState(draw_data, ctx, fr);
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
        D3D12_GPU_DESCRIPTOR_HANDLE texture_handle = {};
        texture_handle.ptr = (UINT64)pcmd->GetTexID();
        ctx->SetGraphicsRootDescriptorTable(1, texture_handle);
        ctx->RSSetScissorRects(1, &r);
        ctx->DrawIndexedInstanced(pcmd->ElemCount, 1, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset, 0);
      }
    }
    global_idx_offset += cmd_list->IdxBuffer.Size;
    global_vtx_offset += cmd_list->VtxBuffer.Size;
  }
}

static void ImGui_ImplDX12_CreateFontsTexture()
{
  // Build texture atlas
  ImGuiIO& io = ImGui::GetIO();
  ImGui_ImplDX12_Data* bd = ImGui_ImplDX12_GetBackendData();
  unsigned char* pixels;
  int width, height;
  io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

  // Upload texture to graphics system
  bd->texture = rex::rhi::create_texture2d((const char*)pixels, DXGI_FORMAT_R8G8B8A8_UNORM, width, height);
  bd->texture_handle = rex::rhi::create_texture2d_srv(bd->texture);

  // Store our identifier
  // READ THIS IF THE STATIC_ASSERT() TRIGGERS:
  // - Important: to compile on 32-bit systems, this backend requires code to be compiled with '#define ImTextureID ImU64'.
  // - This is because we need ImTextureID to carry a 64-bit value and by default ImTextureID is defined as void*.
  // [Solution 1] IDE/msbuild: in "Properties/C++/Preprocessor Definitions" add 'ImTextureID=ImU64' (this is what we do in the 'example_win32_direct12/example_win32_direct12.vcxproj' project file)
  // [Solution 2] IDE/msbuild: in "Properties/C++/Preprocessor Definitions" add 'IMGUI_USER_CONFIG="my_imgui_config.h"' and inside 'my_imgui_config.h' add '#define ImTextureID ImU64' and as many other options as you like.
  // [Solution 3] IDE/msbuild: edit imconfig.h and add '#define ImTextureID ImU64' (prefer solution 2 to create your own config file!)
  // [Solution 4] command-line: add '/D ImTextureID=ImU64' to your cl.exe command-line (this is what we do in the example_win32_direct12/build_win32.bat file)
  static_assert(sizeof(ImTextureID) >= sizeof(bd->texture_handle.get_gpu().ptr), "Can't pack descriptor handle into TexID, 32-bit not supported yet.");
  io.Fonts->SetTexID((ImTextureID)bd->texture_handle.get_gpu().ptr);
}

bool    ImGui_ImplDX12_CreateDeviceObjects()
{
  ImGui_ImplDX12_Data* bd = ImGui_ImplDX12_GetBackendData();
  if (!bd || !bd->pd3dDevice)
    return false;
  if (!bd->pipeline_state.is_valid())
    ImGui_ImplDX12_InvalidateDeviceObjects();

  ImGui_ImplDX12_CreateFontsTexture();

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

  rex::rhi::CompileShaderDesc compile_vs_desc{};
  compile_vs_desc.shader_code = rex::memory::Blob(rsl::make_unique<char[]>(vertex_shader.length()));
  compile_vs_desc.shader_code.write(vertex_shader.data(), vertex_shader.length());
  compile_vs_desc.shader_entry_point = "main";
  compile_vs_desc.shader_feature_target = "vs_5_0";
  compile_vs_desc.shader_name = "imgui_vertex_shader";
  compile_vs_desc.shader_type = rex::rhi::ShaderType::Vertex;
  bd->vertex_shader = rex::rhi::compile_shader(compile_vs_desc);

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

  rex::rhi::CompileShaderDesc compile_ps_desc{};
  compile_ps_desc.shader_code = rex::memory::Blob(rsl::make_unique<char[]>(pixel_shader.length()));
  compile_ps_desc.shader_code.write(pixel_shader.data(), pixel_shader.length());
  compile_ps_desc.shader_entry_point = "main";
  compile_ps_desc.shader_feature_target = "ps_5_0";
  compile_ps_desc.shader_name = "imgui_pixel_shader";
  compile_ps_desc.shader_type = rex::rhi::ShaderType::Pixel;
  bd->pixel_shader = rex::rhi::compile_shader(compile_ps_desc);

  // Link shaders
  rex::rhi::LinkShaderDesc link_shader_desc{};
  link_shader_desc.vertex_shader = bd->vertex_shader;
  link_shader_desc.pixel_shader = bd->pixel_shader;

  // We have 2 constants for the shader, 1 in the vertex shader and 1 in the pixel shader
  link_shader_desc.constants = rsl::make_unique<rex::rhi::ShaderParameterLayoutDescription[]>(1);
  link_shader_desc.constants[0] = { rex::rhi::ShaderParameterType::CBuffer, "vertexBuffer", 0, rex::renderer::ShaderVisibility::Vertex }; // We have 1 constant buffer in the vertex shader

  link_shader_desc.desc_tables = rsl::make_unique<rex::rhi::DescriptorTableDescription[]>(1);
  link_shader_desc.desc_tables[0].ranges = rsl::make_unique<rex::rhi::DescriptorRange[]>(1);
  link_shader_desc.desc_tables[0].ranges[0] = { rex::rhi::DescriptorRangeType::ShaderResourceView, 1 }; // We have 1 src which points to our font texture
  link_shader_desc.desc_tables[0].visibility = rex::renderer::ShaderVisibility::Pixel;

  // We have 1 sampler, used for sampling the font texture
  link_shader_desc.samplers = rsl::make_unique<rex::rhi::ShaderSamplerDescription[]>(1);
  link_shader_desc.samplers[0].filtering = rex::renderer::SamplerFiltering::MinMagMipLinear;
  link_shader_desc.samplers[0].address_mode_u = rex::renderer::TextureAddressMode::Wrap;
  link_shader_desc.samplers[0].address_mode_v = rex::renderer::TextureAddressMode::Wrap;
  link_shader_desc.samplers[0].address_mode_w = rex::renderer::TextureAddressMode::Wrap;
  link_shader_desc.samplers[0].mip_lod_bias = 0.0f;
  link_shader_desc.samplers[0].max_anisotropy = 0;
  link_shader_desc.samplers[0].comparison_func = rex::renderer::ComparisonFunc::Always;
  link_shader_desc.samplers[0].border_color = rex::renderer::BorderColor::TransparentBlack;
  link_shader_desc.samplers[0].min_lod = 0.0f;
  link_shader_desc.samplers[0].max_lod = 0.0f;
  link_shader_desc.samplers[0].shader_register = 0;
  link_shader_desc.samplers[0].register_space = 0;
  link_shader_desc.samplers[0].shader_visibility = rex::renderer::ShaderVisibility::Pixel;

  bd->shader_program = rex::rhi::link_shader(link_shader_desc);

  bd->constant_buffer = rex::rhi::create_constant_buffer(sizeof(VERTEX_CONSTANT_BUFFER_DX12));

  rex::rhi::InputLayoutDesc input_layout_desc;
  input_layout_desc.input_layout =
  {
    rex::rhi::InputLayoutElementDesc { "POSITION",  rex::renderer::VertexBufferFormat::Float2, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 0, 0 },
    rex::rhi::InputLayoutElementDesc { "TEXCOORD",  rex::renderer::VertexBufferFormat::Float2, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 8, 0 },
    rex::rhi::InputLayoutElementDesc { "COLOR", rex::renderer::VertexBufferFormat::UNorm4, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 16, 0 }
  };
  bd->input_layout = rex::rhi::create_input_layout(input_layout_desc);

  // Raster State
  rex::rhi::RasterStateDesc rasterizer_desc{};
  rasterizer_desc.fill_mode = rex::renderer::FillMode::Solid;
  rasterizer_desc.cull_mode = rex::renderer::CullMode::None;
  rasterizer_desc.front_ccw = false;
  rasterizer_desc.depth_bias = D3D12_DEFAULT_DEPTH_BIAS;
  rasterizer_desc.depth_bias_clamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
  rasterizer_desc.sloped_scale_depth_bias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
  rasterizer_desc.depth_clip_enable = true;
  rasterizer_desc.multisample = false;
  rasterizer_desc.aa_lines = false;
  rasterizer_desc.forced_sample_count = 0;

  rex::rhi::PipelineStateDesc pso_desc{};
  pso_desc.input_layout = bd->input_layout;
  pso_desc.raster_state = rex::rhi::create_raster_state(rasterizer_desc);
  pso_desc.shader = bd->shader_program;

  // Blend State
  pso_desc.blend_state = rex::rhi::BlendState();
  rex::rhi::BlendState& blend_state = pso_desc.blend_state.value();
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

  bd->pipeline_state = rex::rhi::create_pso(pso_desc);

  return true;
}

static void ImGui_ImplDX12_DestroyRenderBuffers(ImGui_ImplDX12_RenderBuffers* render_buffers)
{
  render_buffers->IndexBufferSize = render_buffers->VertexBufferSize = 0;
}

void    ImGui_ImplDX12_InvalidateDeviceObjects()
{
  ImGui_ImplDX12_Data* bd = ImGui_ImplDX12_GetBackendData();
  if (!bd || !bd->pd3dDevice)
    return;

  ImGuiIO& io = ImGui::GetIO();
  io.Fonts->SetTexID(0); // We copied bd->pFontTextureView to io.Fonts->TexID so let's clear that as well.
}

//--------------------------------------------------------------------------------------------------------
// MULTI-VIEWPORT / PLATFORM INTERFACE SUPPORT
// This is an _advanced_ and _optional_ feature, allowing the backend to create and handle multiple viewports simultaneously.
// If you are new to dear imgui or creating a new binding for dear imgui, it is recommended that you completely ignore this section first..
//--------------------------------------------------------------------------------------------------------

static void ImGui_ImplDX12_CreateWindow(ImGuiViewport* viewport)
{
  ImGui_ImplDX12_Data* bd = ImGui_ImplDX12_GetBackendData();
  ImGui_ImplDX12_ViewportData* vd = IM_NEW(ImGui_ImplDX12_ViewportData)(bd->numFramesInFlight);
  viewport->RendererUserData = vd;

  // PlatformHandleRaw should always be a HWND, whereas PlatformHandle might be a higher-level handle (e.g. GLFWWindow*, SDL_Window*).
  // Some backends will leave PlatformHandleRaw == 0, in which case we assume PlatformHandle will contain the HWND.
  HWND hwnd = viewport->PlatformHandleRaw ? (HWND)viewport->PlatformHandleRaw : (HWND)viewport->PlatformHandle;
  IM_ASSERT(hwnd != 0);

  vd->FrameIndex = UINT_MAX;
  HRESULT res = S_OK;

  // Create fence.
  // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv REX CODE vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  rex::wrl::ComPtr<ID3D12Fence> fence;
  res = bd->pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf()));
  IM_ASSERT(res == S_OK);

  // Create command queue.
  D3D12_COMMAND_QUEUE_DESC queue_desc = {};
  queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
  queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

  rex::wrl::ComPtr<ID3D12CommandQueue> command_queue;
  res = bd->pd3dDevice->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(command_queue.GetAddressOf()));
  vd->rex_command_queue = rsl::make_unique<rex::rhi::CommandQueue>(command_queue, fence);
  IM_ASSERT(res == S_OK);
  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ REX CODE ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  // Create command allocator.
  for (UINT i = 0; i < bd->numFramesInFlight; ++i)
  {
    rex::wrl::ComPtr<ID3D12CommandAllocator> cmd_alloc;
    res = bd->pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(cmd_alloc.GetAddressOf()));
    vd->FrameCtx[i].command_allocator = rsl::make_unique<rex::rhi::CommandAllocator>(cmd_alloc);
    IM_ASSERT(res == S_OK);
  }

  // Create command list.
  rex::wrl::ComPtr<ID3D12GraphicsCommandList> cmd_list;
  res = bd->pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, vd->FrameCtx[0].command_allocator->get(), nullptr, IID_PPV_ARGS(cmd_list.GetAddressOf()));
  vd->command_list = rsl::make_unique<rex::rhi::CommandList2>(cmd_list);
  IM_ASSERT(res == S_OK);

  // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv REX CODE vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

  // Create swap chain
  // FIXME-VIEWPORT: May want to copy/inherit swap chain settings from the user/application.
  DXGI_SWAP_CHAIN_DESC1 sd1;
  ZeroMemory(&sd1, sizeof(sd1));
  sd1.BufferCount = bd->numFramesInFlight;
  sd1.Width = (UINT)viewport->Size.x;
  sd1.Height = (UINT)viewport->Size.y;
  sd1.Format = bd->RTVFormat;
  sd1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd1.SampleDesc.Count = 1;
  sd1.SampleDesc.Quality = 0;
  sd1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
  sd1.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
  sd1.Scaling = DXGI_SCALING_NONE;
  sd1.Stereo = FALSE;

  IDXGIFactory4* dxgi_factory = nullptr;
  res = ::CreateDXGIFactory1(IID_PPV_ARGS(&dxgi_factory));
  IM_ASSERT(res == S_OK);

  rex::wrl::ComPtr<IDXGISwapChain1> swap_chain = nullptr;
  res = dxgi_factory->CreateSwapChainForHwnd(vd->rex_command_queue->get(), hwnd, &sd1, nullptr, nullptr, &swap_chain);
  IM_ASSERT(res == S_OK);

  dxgi_factory->Release();

  // Or swapChain.As(&mSwapChain)
  rex::wrl::ComPtr<IDXGISwapChain3> d3d_swapchain_3;
  swap_chain->QueryInterface(IID_PPV_ARGS(&d3d_swapchain_3));

  // Create the render targets
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    desc.NumDescriptors = bd->numFramesInFlight;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    desc.NodeMask = 1;

    rex::wrl::ComPtr<ID3D12DescriptorHeap> desc_heap;
    HRESULT hr = bd->pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&desc_heap));
    IM_ASSERT(hr == S_OK);
    vd->rex_descriptor_heap = rsl::make_unique<rex::rhi::DescriptorHeap>(desc_heap, bd->pd3dDevice);

    SIZE_T rtv_descriptor_size = bd->pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = vd->rex_descriptor_heap->get()->GetCPUDescriptorHandleForHeapStart();
    for (UINT i = 0; i < bd->numFramesInFlight; i++)
    {
      vd->FrameCtx[i].RenderTargetCpuDescriptors = rtv_handle;
      rtv_handle.ptr += rtv_descriptor_size;
    }

    vd->rex_swapchain = rsl::make_unique<rex::rhi::Swapchain>(d3d_swapchain_3, sd1.Format, sd1.BufferCount, vd->rex_descriptor_heap.get(), nullptr, nullptr);
    ID3D12Resource* back_buffer;
    for (UINT i = 0; i < bd->numFramesInFlight; i++)
    {
      IM_ASSERT(vd->FrameCtx[i].RenderTarget == nullptr);
      vd->rex_swapchain->get()->GetBuffer(i, IID_PPV_ARGS(&back_buffer));
      bd->pd3dDevice->CreateRenderTargetView(back_buffer, nullptr, vd->FrameCtx[i].RenderTargetCpuDescriptors);
      vd->FrameCtx[i].RenderTarget = back_buffer;
    }
  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ REX CODE ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


  for (UINT i = 0; i < bd->numFramesInFlight; i++)
    ImGui_ImplDX12_DestroyRenderBuffers(&vd->FrameRenderBuffers[i]);
}

static void ImGui_WaitForPendingOperations(ImGui_ImplDX12_ViewportData* vd)
{
  HRESULT hr = S_FALSE;
  if (vd && vd->rex_command_queue /*&& vd->Fence && vd->FenceEvent*/)
  {
    vd->rex_command_queue->flush();
    ++vd->FenceSignaledValue;
  }
}

static void ImGui_ImplDX12_DestroyWindow(ImGuiViewport* viewport)
{
  // The main viewport (owned by the application) will always have RendererUserData == 0 since we didn't create the data for it.
  ImGui_ImplDX12_Data* bd = ImGui_ImplDX12_GetBackendData();
  if (ImGui_ImplDX12_ViewportData* vd = (ImGui_ImplDX12_ViewportData*)viewport->RendererUserData)
  {
    ImGui_WaitForPendingOperations(vd);

    for (UINT i = 0; i < bd->numFramesInFlight; i++)
    {
      SafeRelease(vd->FrameCtx[i].RenderTarget);
      ImGui_ImplDX12_DestroyRenderBuffers(&vd->FrameRenderBuffers[i]);
    }
    IM_DELETE(vd);
  }
  viewport->RendererUserData = nullptr;
}

static void ImGui_ImplDX12_SetWindowSize(ImGuiViewport* viewport, ImVec2 size)
{
  ImGui_ImplDX12_Data* bd = ImGui_ImplDX12_GetBackendData();
  ImGui_ImplDX12_ViewportData* vd = (ImGui_ImplDX12_ViewportData*)viewport->RendererUserData;

  ImGui_WaitForPendingOperations(vd);

  for (UINT i = 0; i < bd->numFramesInFlight; i++)
    SafeRelease(vd->FrameCtx[i].RenderTarget);

  if (vd->rex_swapchain)
  {
    ID3D12Resource* back_buffer = nullptr;
    vd->rex_swapchain->get()->ResizeBuffers(0, (UINT)size.x, (UINT)size.y, DXGI_FORMAT_UNKNOWN, 0);
    for (UINT i = 0; i < bd->numFramesInFlight; i++)
    {
      vd->rex_swapchain->get()->GetBuffer(i, IID_PPV_ARGS(&back_buffer));
      bd->pd3dDevice->CreateRenderTargetView(back_buffer, nullptr, vd->FrameCtx[i].RenderTargetCpuDescriptors);
      vd->FrameCtx[i].RenderTarget = back_buffer;
    }
  }
}

static void ImGui_ImplDX12_RenderWindow(ImGuiViewport* viewport, void*)
{
  ImGui_ImplDX12_Data* bd = ImGui_ImplDX12_GetBackendData();
  ImGui_ImplDX12_ViewportData* vd = (ImGui_ImplDX12_ViewportData*)viewport->RendererUserData;

  ImGui_ImplDX12_FrameContext* frame_context = &vd->FrameCtx[vd->FrameIndex % bd->numFramesInFlight];
  UINT back_buffer_idx = vd->rex_swapchain->get()->GetCurrentBackBufferIndex();

  const ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
  D3D12_RESOURCE_BARRIER barrier = {};
  barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
  barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
  barrier.Transition.pResource = vd->FrameCtx[back_buffer_idx].RenderTarget;
  barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
  barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
  barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

  // Draw

  vd->command_list->start_recording_commands(frame_context->command_allocator.get());

  ID3D12GraphicsCommandList* cmd_list = vd->command_list->get();
  cmd_list->ResourceBarrier(1, &barrier);
  cmd_list->OMSetRenderTargets(1, &vd->FrameCtx[back_buffer_idx].RenderTargetCpuDescriptors, FALSE, nullptr);
  if (!(viewport->Flags & ImGuiViewportFlags_NoRendererClear))
    cmd_list->ClearRenderTargetView(vd->FrameCtx[back_buffer_idx].RenderTargetCpuDescriptors, (float*)&clear_color, 0, nullptr);
  ID3D12DescriptorHeap* desc_heap = bd->pd3dSrvDescHeap->get();
  cmd_list->SetDescriptorHeaps(1, &desc_heap);

  ImGui_ImplDX12_RenderDrawData(viewport->DrawData, cmd_list);

  barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
  barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
  cmd_list->ResourceBarrier(1, &barrier);
  vd->command_list->stop_recording_commands();

  vd->rex_command_queue->wait(vd->FenceSignaledValue);
  vd->rex_command_queue->execute(cmd_list);
  vd->rex_command_queue->inc_fence();
  vd->rex_command_queue->flush();
  ++vd->FenceSignaledValue;
}

static void ImGui_ImplDX12_SwapBuffers(ImGuiViewport* viewport, void*)
{
  ImGui_ImplDX12_ViewportData* vd = (ImGui_ImplDX12_ViewportData*)viewport->RendererUserData;

  vd->rex_swapchain->get()->Present(0, 0);
  while (vd->rex_command_queue->fence_value() < vd->FenceSignaledValue)
    ::SwitchToThread();
}

void ImGui_ImplDX12_InitPlatformInterface()
{
  ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
  platform_io.Renderer_CreateWindow = ImGui_ImplDX12_CreateWindow;
  platform_io.Renderer_DestroyWindow = ImGui_ImplDX12_DestroyWindow;
  platform_io.Renderer_SetWindowSize = ImGui_ImplDX12_SetWindowSize;
  platform_io.Renderer_RenderWindow = ImGui_ImplDX12_RenderWindow;
  platform_io.Renderer_SwapBuffers = ImGui_ImplDX12_SwapBuffers;
}

void ImGui_ImplDX12_ShutdownPlatformInterface()
{
  ImGui::DestroyPlatformWindows();
}

//-----------------------------------------------------------------------------

namespace rex
{
  namespace renderer
  {
    ImGuiRenderer::ImGuiRenderer(ID3D12Device1* device, s32 numFramesInFlight, DXGI_FORMAT rtvFormat, HWND hwnd)
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

      io.BackendRendererName = "DirectX 12 ImGui Renderer";
      io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

      ImGui_ImplWin32_Init(hwnd);

      IM_ASSERT(io.BackendRendererUserData == nullptr && "Already initialized a renderer backend!");

      // Setup backend capabilities flags
      ImGui_ImplDX12_Data* bd = IM_NEW(ImGui_ImplDX12_Data)();
      io.BackendRendererUserData = (void*)bd;
      io.BackendRendererName = "imgui_impl_dx12";
      io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
      io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;  // We can create multi-viewports on the Renderer side (optional)
      if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        ImGui_ImplDX12_InitPlatformInterface();

      bd->pd3dDevice = device;
      bd->RTVFormat = rtvFormat;

      bd->numFramesInFlight = numFramesInFlight;
      bd->pd3dSrvDescHeap = rhi::get_cbv_uav_srv_heap();

      // Create a dummy ImGui_ImplDX12_ViewportData holder for the main viewport,
      // Since this is created and managed by the application, we will only use the ->Resources[] fields.
      ImGuiViewport* main_viewport = ImGui::GetMainViewport();
      main_viewport->RendererUserData = IM_NEW(ImGui_ImplDX12_ViewportData)(bd->numFramesInFlight);
    }
    ImGuiRenderer::~ImGuiRenderer()
    {
      ImGui_ImplDX12_Data* bd = ImGui_ImplDX12_GetBackendData();
      IM_ASSERT(bd != nullptr && "No renderer backend to shutdown, or already shutdown?");
      ImGuiIO& io = ImGui::GetIO();

      // Manually delete main viewport render resources in-case we haven't initialized for viewports
      ImGuiViewport* main_viewport = ImGui::GetMainViewport();
      if (ImGui_ImplDX12_ViewportData* vd = (ImGui_ImplDX12_ViewportData*)main_viewport->RendererUserData)
      {
        // We could just call ImGui_ImplDX12_DestroyWindow(main_viewport) as a convenience but that would be misleading since we only use data->Resources[]
        for (UINT i = 0; i < bd->numFramesInFlight; i++)
          ImGui_ImplDX12_DestroyRenderBuffers(&vd->FrameRenderBuffers[i]);
        IM_DELETE(vd);
        main_viewport->RendererUserData = nullptr;
      }

      // Clean up windows and device objects
      ImGui_ImplDX12_ShutdownPlatformInterface();
      ImGui_ImplDX12_InvalidateDeviceObjects();

      io.BackendRendererName = nullptr;
      io.BackendRendererUserData = nullptr;
      io.BackendFlags &= ~(ImGuiBackendFlags_RendererHasVtxOffset | ImGuiBackendFlags_RendererHasViewports);
      IM_DELETE(bd);
    }

    void ImGuiRenderer::new_frame()
    {
      ImGui_ImplWin32_NewFrame();

      ImGui_ImplDX12_Data* bd = ImGui_ImplDX12_GetBackendData();
      IM_ASSERT(bd != nullptr && "Did you call ImGui_ImplDX12_Init()?");

      if (!bd->pipeline_state.is_valid())
        ImGui_ImplDX12_CreateDeviceObjects();

      ImGui::NewFrame();
    }

    void ImGuiRenderer::render()
    {
      ImGui::Render();

      ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), rhi::cmd_list());

      if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
      {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault(nullptr, (void*)rhi::cmd_list());
      }
    }

    Error ImGuiRenderer::init_input_layout()
    {
      return Error::no_error();
    }
    Error ImGuiRenderer::init_shader()
    {
      return Error::no_error();
    }
    Error ImGuiRenderer::init_font_texture()
    {
      return Error::no_error();
    }
    Error ImGuiRenderer::init_buffers()
    {
      return Error::no_error();
    }
  }
}

#endif // #ifndef IMGUI_DISABLE
