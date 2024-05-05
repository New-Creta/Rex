#include "imgui/imgui.h"
#ifndef IMGUI_DISABLE
#include "rex_directx/rendering/dx_imgui_renderer.h"

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
#include "rex_directx/diagnostics/dx_call.h"

DEFINE_LOG_CATEGORY(LogImgui);

// First render the main window widgets
// Next render all the child windows

namespace rex
{
  namespace renderer
  {
    // Buffers used during the rendering of a frame
    struct ImGuiRenderBuffer
    {
      s32                 IndexBufferSize;    // the number of indices the index buffer supports
      s32                 VertexBufferSize;   // the number of vertices the vertex buffer supports

      rex::rhi::ResourceSlot vertex_buffer;   // resource slot of the vertex buffer
      rex::rhi::ResourceSlot index_buffer;    // resource slot of the index buffer
    };

    // We support multiple frames in flight at the same time.
    // Therefore we need some resources that are tied to each frame that's currently in flight
    class ImGuiFrameContext
    {
    public:
      ImGuiFrameContext(ID3D12Device1* device)
      {
        rex::wrl::ComPtr<ID3D12CommandAllocator> cmd_alloc;
        DX_CALL(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(cmd_alloc.GetAddressOf())));
        m_command_allocator = rsl::make_unique<rex::rhi::CommandAllocator>(cmd_alloc);
      }

      rex::rhi::CommandAllocator* allocator()
      {
        return m_command_allocator.get();
      }

    private:
      rsl::unique_ptr<rex::rhi::CommandAllocator> m_command_allocator;
    };

    struct ImGuiVertexConstantBuffer
    {
      float   mvp[4][4];
    };

    // Helper structure we store in the void* RendererUserData field of each ImGuiViewport to easily retrieve our backend data.
    // Main viewport created by application will only use the Resources field.
    // Secondary viewports created by this backend will use all the fields (including Window fields),
    class ImGuiViewportData
    {
    public:
      ImGuiViewportData(ImGuiViewport* viewport, ID3D12Device1* device, s32 maxNumFramesInFlight, DXGI_FORMAT rtvFormat, const rhi::ResourceSlot& shaderProgram, const rhi::ResourceSlot& pso, const rhi::ResourceSlot& cb)
        : m_imgui_viewport(viewport)
        , m_fence_signaled_value(0)
        , m_max_num_frames_in_flight(maxNumFramesInFlight)
        , m_frame_idx(0)
        , m_shader_program(shaderProgram)
        , m_pipeline_state(pso)
        , m_constant_buffer(cb)
        , m_command_list(nullptr)
      {
        // PlatformHandleRaw should always be a HWND, whereas PlatformHandle might be a higher-level handle (e.g. GLFWWindow*, SDL_Window*).
        // Some backends will leave PlatformHandleRaw == 0, in which case we assume PlatformHandle will contain the HWND.
        HWND hwnd = viewport->PlatformHandleRaw ? (HWND)viewport->PlatformHandleRaw : (HWND)viewport->PlatformHandle;
        IM_ASSERT(hwnd != 0);

        init_command_queue(device);
        init_frame_contexts(device);
        init_desc_heap(device);

        if (viewport != ImGui::GetMainViewport())
        {
          init_cmd_list(device);
          init_swapchain(viewport, device, hwnd, rtvFormat);
        }
      }

      void wait_for_pending_operations()
      {
        m_command_queue->flush();
        ++m_fence_signaled_value;
      }

      void begin_draw()
      {
        if (m_swapchain)
        {
          m_command_list->start_recording_commands(current_frame_ctx()->allocator());
          m_swapchain->transition_backbuffer(m_command_list->get(), D3D12_RESOURCE_STATE_RENDER_TARGET);
          m_command_list->get()->OMSetRenderTargets(1, &m_swapchain->backbuffer_view().get(), FALSE, nullptr);
        }
      }
      void end_draw()
      {
        if (m_swapchain)
        {
          m_swapchain->transition_backbuffer(m_command_list->get(), D3D12_RESOURCE_STATE_PRESENT);
          m_command_list->stop_recording_commands();

          m_command_queue->wait(m_fence_signaled_value);
          m_command_queue->execute(m_command_list->get());
          m_command_queue->inc_fence();
          m_command_queue->flush();
          ++m_fence_signaled_value;
        }
      }

      void clear_render_target(const ImVec4& clearColor)
      {
        ID3D12GraphicsCommandList* cmdlist = m_command_list
          ? m_command_list->get()
          : rhi::cmd_list()->get();
        cmdlist->ClearRenderTargetView(m_swapchain->backbuffer_view().get(), (f32*)&clearColor, 0, nullptr);
      }
      void draw(ImDrawData* drawData, rex::rhi::DescriptorHeap* srvDescHeap)
      {
        ID3D12GraphicsCommandList* cmdlist = m_command_list
          ? m_command_list->get()
          : rhi::cmd_list()->get();

        ID3D12DescriptorHeap* desc_heap = srvDescHeap->get();
        cmdlist->SetDescriptorHeaps(1, &desc_heap);
        render_draw_data();
      }

      void resize_buffers(s32 width, s32 height)
      {
        m_swapchain->resize_buffers(width, height, (DXGI_SWAP_CHAIN_FLAG)0);

      }
      void present()
      {
        m_swapchain->get()->Present(0, 0);
      }
      void yield_thread()
      {
        while (m_command_queue->fence_value() < m_fence_signaled_value)
          ::SwitchToThread();
      }

    private:
      Error init_command_queue(ID3D12Device1* device)
      {
        // Create fence.
        rex::wrl::ComPtr<ID3D12Fence> fence;
        DX_CALL(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf())));

        // Create command queue.
        D3D12_COMMAND_QUEUE_DESC queue_desc = {};
        queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

        rex::wrl::ComPtr<ID3D12CommandQueue> command_queue;
        DX_CALL(device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(command_queue.GetAddressOf())));
        m_command_queue = rsl::make_unique<rex::rhi::CommandQueue>(command_queue, fence);

        return Error::no_error();
      }
      Error init_frame_contexts(ID3D12Device1* device)
      {
        // Create command allocator.
        m_frame_ctx = rsl::make_unique<rsl::unique_ptr<ImGuiFrameContext>[]>(m_max_num_frames_in_flight);
        m_render_buffers = rsl::make_unique<rsl::unique_ptr<ImGuiRenderBuffer>[]>(m_max_num_frames_in_flight);
        for (UINT i = 0; i < m_max_num_frames_in_flight; ++i)
        {
          m_frame_ctx[i] = rsl::make_unique<ImGuiFrameContext>(device);
          m_render_buffers[i] = rsl::make_unique<ImGuiRenderBuffer>();
        }

        return Error::no_error();
      }
      Error init_cmd_list(ID3D12Device1* device)
      {
        // Create command list.
        rex::wrl::ComPtr<ID3D12GraphicsCommandList> cmd_list;
        DX_CALL(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_frame_ctx[0]->allocator()->get(), nullptr, IID_PPV_ARGS(cmd_list.GetAddressOf())));
        m_command_list = rsl::make_unique<rex::rhi::CommandList2>(cmd_list);

        return Error::no_error();
      }
      Error init_desc_heap(ID3D12Device1* device)
      {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        desc.NumDescriptors = m_max_num_frames_in_flight;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NodeMask = 1;

        rex::wrl::ComPtr<ID3D12DescriptorHeap> desc_heap;
        DX_CALL(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&desc_heap)));
        m_descriptor_heap = rsl::make_unique<rex::rhi::DescriptorHeap>(desc_heap, device);

        return Error::no_error();
      }
      Error init_swapchain(ImGuiViewport* viewport, ID3D12Device1* device, HWND hwnd, DXGI_FORMAT rtvFormat)
      {
        // Create swap chain
        // FIXME-VIEWPORT: May want to copy/inherit swap chain settings from the user/application.
        DXGI_SWAP_CHAIN_DESC1 sd1;
        ZeroMemory(&sd1, sizeof(sd1));
        sd1.BufferCount = m_max_num_frames_in_flight;
        sd1.Width = (UINT)viewport->Size.x;
        sd1.Height = (UINT)viewport->Size.y;
        sd1.Format = rtvFormat;
        sd1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd1.SampleDesc.Count = 1;
        sd1.SampleDesc.Quality = 0;
        sd1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd1.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        sd1.Scaling = DXGI_SCALING_NONE;
        sd1.Stereo = FALSE;

        wrl::ComPtr<IDXGIFactory4> dxgi_factory = nullptr;
        DX_CALL(CreateDXGIFactory1(IID_PPV_ARGS(dxgi_factory.GetAddressOf())));

        rex::wrl::ComPtr<IDXGISwapChain1> swap_chain = nullptr;
        DX_CALL(dxgi_factory->CreateSwapChainForHwnd(m_command_queue->get(), hwnd, &sd1, nullptr, nullptr, &swap_chain));

        // Or swapChain.As(&mSwapChain)
        rex::wrl::ComPtr<IDXGISwapChain3> d3d_swapchain_3;
        swap_chain->QueryInterface(IID_PPV_ARGS(&d3d_swapchain_3));

        m_swapchain = rsl::make_unique<rex::rhi::Swapchain>(d3d_swapchain_3, sd1.Format, sd1.BufferCount, m_descriptor_heap.get(), nullptr, nullptr);

        return Error::no_error();
      }

      void update_to_next_frame_ctx()
      {
        ++m_frame_idx;
        if (m_frame_idx == m_max_num_frames_in_flight)
        {
          m_frame_idx = 0;
        }
      }
      ImGuiFrameContext* current_frame_ctx()
      {
        return m_frame_ctx[m_frame_idx].get();
      }
      void setup_render_state(ImDrawData* drawData, ID3D12GraphicsCommandList* ctx, class ImGuiRenderBuffer* fr)
      {
        // Setup orthographic projection matrix into our constant buffer
        // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right).
        ImGuiVertexConstantBuffer vertex_constant_buffer;
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
        D3D12_VIEWPORT vp;
        memset(&vp, 0, sizeof(D3D12_VIEWPORT));
        vp.Width = drawData->DisplaySize.x;
        vp.Height = drawData->DisplaySize.y;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = vp.TopLeftY = 0.0f;
        ctx->RSSetViewports(1, &vp);

        rex::rhi::set_vertex_buffer(fr->vertex_buffer, ctx);
        rex::rhi::set_index_buffer(fr->index_buffer, ctx);

        ctx->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        ctx->SetPipelineState(rex::rhi::get_pso(m_pipeline_state)->get());
        ctx->SetGraphicsRootSignature(rex::rhi::get_shader(m_shader_program)->root_signature());

        rex::rhi::update_buffer(m_constant_buffer, &vertex_constant_buffer, sizeof(vertex_constant_buffer), ctx);
        rex::rhi::set_constant_buffer(0, m_constant_buffer, ctx);

        // Setup blend factor
        const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
        ctx->OMSetBlendFactor(blend_factor);
      }

      void render_draw_data()
      {
        ImDrawData* drawData = m_imgui_viewport->DrawData;

        // Avoid rendering when minimized
        if (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f)
          return;

        if (drawData->CmdListsCount == 0)
          return;

        ImGuiViewportData* vd = this;
        update_to_next_frame_ctx();
        ImGuiRenderBuffer* fr = m_render_buffers[m_frame_idx % m_max_num_frames_in_flight].get();

        //vd->FrameIndex++;
        //ImGuiRenderBuffer* fr = &vd->FrameRenderBuffers[vd->FrameIndex % m_max_num_frames_in_flight];

        // Create and grow vertex/index buffers if needed
        if (!fr->vertex_buffer.is_valid() || fr->VertexBufferSize < drawData->TotalVtxCount)
        {
          fr->VertexBufferSize = drawData->TotalVtxCount + 5000;
          fr->vertex_buffer = rex::rhi::create_vertex_buffer(sizeof(ImDrawVert) * fr->VertexBufferSize, sizeof(ImDrawVert));
        }
        if (!fr->index_buffer.is_valid() || fr->IndexBufferSize < drawData->TotalIdxCount)
        {
          rex::renderer::IndexBufferFormat format = sizeof(ImDrawIdx) == 2
            ? rex::renderer::IndexBufferFormat::Uint16
            : rex::renderer::IndexBufferFormat::Uint32;
          fr->IndexBufferSize = drawData->TotalIdxCount + 10000;
          fr->index_buffer = rex::rhi::create_index_buffer(fr->IndexBufferSize * sizeof(ImDrawIdx), format);
        }

        // Upload vertex/index data into a single contiguous GPU buffer
        s32 vtx_offset = 0;
        s32 idx_offset = 0;

        ID3D12GraphicsCommandList* cmdlist = m_command_list
          ? m_command_list->get()
          : rhi::cmd_list()->get();

        for (int n = 0; n < drawData->CmdListsCount; n++)
        {
          const ImDrawList* cmd_list = drawData->CmdLists[n];
          rex::rhi::update_buffer(fr->vertex_buffer, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), cmdlist, vtx_offset);
          rex::rhi::update_buffer(fr->index_buffer, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), cmdlist, idx_offset);
          vtx_offset += cmd_list->VtxBuffer.Size * sizeof(ImDrawVert);
          idx_offset += cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);
        }

        // Setup desired DX state
        setup_render_state(drawData, cmdlist, fr);

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
                setup_render_state(drawData, cmdlist, fr);
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
              cmdlist->SetGraphicsRootDescriptorTable(1, texture_handle);
              cmdlist->RSSetScissorRects(1, &r);
              cmdlist->DrawIndexedInstanced(pcmd->ElemCount, 1, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset, 0);
            }
          }
          global_idx_offset += cmd_list->IdxBuffer.Size;
          global_vtx_offset += cmd_list->VtxBuffer.Size;
        }
      }

    private:
      rsl::unique_ptr<rhi::CommandList2> m_command_list;
      rsl::unique_array<rsl::unique_ptr<ImGuiFrameContext>> m_frame_ctx;
      rsl::unique_array<rsl::unique_ptr<ImGuiRenderBuffer>> m_render_buffers;
      rsl::unique_ptr<rhi::CommandQueue> m_command_queue;
      rsl::unique_ptr<rhi::DescriptorHeap> m_descriptor_heap;
      rsl::unique_ptr<rhi::Swapchain> m_swapchain;
      ImGuiViewport* m_imgui_viewport;

      rex::rhi::ResourceSlot m_shader_program;
      rex::rhi::ResourceSlot m_pipeline_state;
      rex::rhi::ResourceSlot m_constant_buffer;

      s64 m_fence_signaled_value;
      s32 m_max_num_frames_in_flight;
      s32 m_frame_idx;
    };

    ImGuiRenderer* g_imgui_renderer = nullptr;

    void create_window_callback(ImGuiViewport* viewport)
    {
      g_imgui_renderer->create_window(viewport);
    }
    void destroy_window_callback(ImGuiViewport* viewport)
    {
      g_imgui_renderer->destroy_window(viewport);
    }
    void set_window_size_callback(ImGuiViewport* viewport, ImVec2 size)
    {
      g_imgui_renderer->set_window_size(viewport, size);
    }
    void render_window_callback(ImGuiViewport* viewport, void* /*renderArg*/)
    {
      g_imgui_renderer->render_window(viewport);
    }
    void swap_buffers_callback(ImGuiViewport* viewport, void* /*renderArg*/)
    {
      g_imgui_renderer->swap_buffers(viewport);
    }

    ImGuiRenderer::ImGuiRenderer(ID3D12Device1* device, s32 numFramesInFlight, DXGI_FORMAT rtvFormat, HWND hwnd)
      : m_device(device)
      , m_rtv_format(rtvFormat)
      , m_max_num_frames_in_flight(numFramesInFlight)
      , m_srv_desc_heap(rhi::get_cbv_uav_srv_heap())
    {
      REX_ASSERT_X(g_imgui_renderer == nullptr, "You can only have 1 imgui renderer");
      g_imgui_renderer = this;

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
      io.BackendRendererUserData = (void*)this;
      io.BackendRendererName = "imgui_impl_dx12";
      io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
      io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;  // We can create multi-viewports on the Renderer side (optional)
      if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        init_platform_interface();

      if (!init_device_objects())
      {
        REX_ERROR(LogImgui, "Failed to create imgui device objects");
      }

      // Create a dummy ImGui_ImplDX12_ViewportData holder for the main viewport,
      // Since this is created and managed by the application, we will only use the ->Resources[] fields.
      ImGuiViewport* main_viewport = ImGui::GetMainViewport();
      main_viewport->RendererUserData = IM_NEW(ImGuiViewportData)(main_viewport, m_device, m_max_num_frames_in_flight, m_rtv_format, m_shader_program, m_pipeline_state, m_constant_buffer);

    }
    ImGuiRenderer::~ImGuiRenderer()
    {
      ImGuiIO& io = ImGui::GetIO();

      // Manually delete main viewport render resources in-case we haven't initialized for viewports
      ImGuiViewport* main_viewport = ImGui::GetMainViewport();
      if (ImGuiViewportData* vd = (ImGuiViewportData*)main_viewport->RendererUserData)
      {
        // We could just call ImGui_ImplDX12_DestroyWindow(main_viewport) as a convenience but that would be misleading since we only use data->Resources[]
        IM_DELETE(vd);
        main_viewport->RendererUserData = nullptr;
      }

      // Clean up windows and device objects
      ImGui::DestroyPlatformWindows();

      io.BackendRendererName = nullptr;
      io.BackendRendererUserData = nullptr;
      io.BackendFlags &= ~(ImGuiBackendFlags_RendererHasVtxOffset | ImGuiBackendFlags_RendererHasViewports);
    }

    void ImGuiRenderer::new_frame()
    {
      ImGui_ImplWin32_NewFrame();

      ImGui::NewFrame();
    }

    void ImGuiRenderer::render()
    {
      ImGui::Render();

      ImGuiViewport* main_viewport = ImGui::GetMainViewport();
      if (ImGuiViewportData* vd = (ImGuiViewportData*)main_viewport->RendererUserData)
      {
        vd->begin_draw();
        vd->draw(main_viewport->DrawData, m_srv_desc_heap);
        //vd->end_draw();
      }

      if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
      {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault(nullptr, (void*)rhi::cmd_list());
      }
    }

    void ImGuiRenderer::end_frame()
    {
      ImGuiViewport* main_viewport = ImGui::GetMainViewport();
      if (ImGuiViewportData* vd = (ImGuiViewportData*)main_viewport->RendererUserData)
      {
        vd->end_draw();
      }
    }

    Error ImGuiRenderer::init_device_objects()
    {
      Error err = Error::no_error();

      err = init_font_texture();
      if (err)
      {
        return Error::create_with_log(LogImgui, "Failed to create font texture");
      }

      err = init_shader();
      if (err)
      {
        return Error::create_with_log(LogImgui, "Failed to create imgui shader");
      }

      err = init_input_layout();
      if (err)
      {
        return Error::create_with_log(LogImgui, "Failed to create imgui input layout");
      }

      err = init_buffers();
      if (err)
      {
        return Error::create_with_log(LogImgui, "Failed to create imgui resource buffers");
      }

      err = init_pso();
      if (err)
      {
        return Error::create_with_log(LogImgui, "Failed to create imgui pso");
      }

      return err;
    }

    void ImGuiRenderer::init_platform_interface()
    {
      ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
      platform_io.Renderer_CreateWindow = create_window_callback;
      platform_io.Renderer_DestroyWindow = destroy_window_callback;
      platform_io.Renderer_SetWindowSize = set_window_size_callback;
      platform_io.Renderer_RenderWindow = render_window_callback;
      platform_io.Renderer_SwapBuffers = swap_buffers_callback;
    }

    Error ImGuiRenderer::init_input_layout()
    {
      rex::rhi::InputLayoutDesc input_layout_desc;
      input_layout_desc.input_layout =
      {
        rex::rhi::InputLayoutElementDesc { "POSITION",  rex::renderer::VertexBufferFormat::Float2, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 0, 0 },
        rex::rhi::InputLayoutElementDesc { "TEXCOORD",  rex::renderer::VertexBufferFormat::Float2, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 8, 0 },
        rex::rhi::InputLayoutElementDesc { "COLOR", rex::renderer::VertexBufferFormat::UNorm4, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 16, 0 }
      };
      m_input_layout = rex::rhi::create_input_layout(input_layout_desc);

      return Error::no_error();
    }
    Error ImGuiRenderer::init_shader()
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

      rex::rhi::CompileShaderDesc compile_vs_desc{};
      compile_vs_desc.shader_code = rex::memory::Blob(rsl::make_unique<char[]>(vertex_shader.length()));
      compile_vs_desc.shader_code.write(vertex_shader.data(), vertex_shader.length());
      compile_vs_desc.shader_entry_point = "main";
      compile_vs_desc.shader_feature_target = "vs_5_0";
      compile_vs_desc.shader_name = "imgui_vertex_shader";
      compile_vs_desc.shader_type = rex::rhi::ShaderType::Vertex;
      m_vertex_shader = rex::rhi::compile_shader(compile_vs_desc);

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
      m_pixel_shader = rex::rhi::compile_shader(compile_ps_desc);

      // Link shaders
      rex::rhi::LinkShaderDesc link_shader_desc{};
      link_shader_desc.vertex_shader = m_vertex_shader;
      link_shader_desc.pixel_shader = m_pixel_shader;

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

      m_shader_program = rex::rhi::link_shader(link_shader_desc);

      return Error::no_error();
    }
    Error ImGuiRenderer::init_font_texture()
    {
      // Build texture atlas
      ImGuiIO& io = ImGui::GetIO();
      //ImGui_ImplDX12_Data* bd = ImGui_ImplDX12_GetBackendData();
      unsigned char* pixels;
      int width, height;
      io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

      // Upload texture to graphics system
      m_texture = rex::rhi::create_texture2d((const char*)pixels, DXGI_FORMAT_R8G8B8A8_UNORM, width, height);
      m_texture_handle = rex::rhi::create_texture2d_srv(m_texture);

      // Store our identifier
      // READ THIS IF THE STATIC_ASSERT() TRIGGERS:
      // - Important: to compile on 32-bit systems, this backend requires code to be compiled with '#define ImTextureID ImU64'.
      // - This is because we need ImTextureID to carry a 64-bit value and by default ImTextureID is defined as void*.
      // [Solution 1] IDE/msbuild: in "Properties/C++/Preprocessor Definitions" add 'ImTextureID=ImU64' (this is what we do in the 'example_win32_direct12/example_win32_direct12.vcxproj' project file)
      // [Solution 2] IDE/msbuild: in "Properties/C++/Preprocessor Definitions" add 'IMGUI_USER_CONFIG="my_imgui_config.h"' and inside 'my_imgui_config.h' add '#define ImTextureID ImU64' and as many other options as you like.
      // [Solution 3] IDE/msbuild: edit imconfig.h and add '#define ImTextureID ImU64' (prefer solution 2 to create your own config file!)
      // [Solution 4] command-line: add '/D ImTextureID=ImU64' to your cl.exe command-line (this is what we do in the example_win32_direct12/build_win32.bat file)
      static_assert(sizeof(ImTextureID) >= sizeof(m_texture_handle.get_gpu().ptr), "Can't pack descriptor handle into TexID, 32-bit not supported yet.");
      io.Fonts->SetTexID((ImTextureID)m_texture_handle.get_gpu().ptr);

      return Error::no_error();
    }
    Error ImGuiRenderer::init_buffers()
    {
      m_constant_buffer = rex::rhi::create_constant_buffer(sizeof(ImGuiVertexConstantBuffer));

      return Error::no_error();
    }
    Error ImGuiRenderer::init_pso()
    {
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
      pso_desc.input_layout = m_input_layout;
      pso_desc.raster_state = rex::rhi::create_raster_state(rasterizer_desc);
      pso_desc.shader = m_shader_program;

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

      return Error::no_error();
    }

    void ImGuiRenderer::create_window(ImGuiViewport* viewport)
    {
      ImGuiViewportData* vd = IM_NEW(ImGuiViewportData)(viewport, m_device, m_max_num_frames_in_flight, m_rtv_format, m_shader_program, m_pipeline_state, m_constant_buffer);
      viewport->RendererUserData = vd;
    }
    void ImGuiRenderer::destroy_window(ImGuiViewport* viewport)
    {
      if (ImGuiViewportData* vd = (ImGuiViewportData*)viewport->RendererUserData)
      {
        vd->wait_for_pending_operations();
        IM_DELETE(vd);
      }
      viewport->RendererUserData = nullptr;
    }

    void ImGuiRenderer::render_window(ImGuiViewport* viewport)
    {
      //ImGui_ImplDX12_Data* bd = ImGui_ImplDX12_GetBackendData();
      ImGuiViewportData* vd = (ImGuiViewportData*)viewport->RendererUserData;

      vd->begin_draw();

      if (!(viewport->Flags & ImGuiViewportFlags_NoRendererClear))
      {
        const ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        vd->clear_render_target(clear_color);
      }

      vd->draw(viewport->DrawData, m_srv_desc_heap);
      vd->end_draw();
    }

    void ImGuiRenderer::set_window_size(ImGuiViewport* viewport, ImVec2 size)
    {
      ImGuiViewportData* vd = (ImGuiViewportData*)viewport->RendererUserData;
      vd->wait_for_pending_operations();
      vd->resize_buffers(size.x, size.y);
    }
    
    void ImGuiRenderer::swap_buffers(ImGuiViewport* viewport)
    {
      ImGuiViewportData* vd = (ImGuiViewportData*)viewport->RendererUserData;
      
      vd->present();
      vd->yield_thread();
    }
  }
}

#endif // #ifndef IMGUI_DISABLE
