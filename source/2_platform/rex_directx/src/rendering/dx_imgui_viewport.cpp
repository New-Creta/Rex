#include "rex_directx/rendering/dx_imgui_viewport.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_directx/system/dx_rhi.h"
#include "rex_directx/resources/dx_shader_program_resource.h"

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

    ImGuiViewport::ImGuiViewport(::ImGuiViewport* viewport, ID3D12Device1* device, s32 maxNumFramesInFlight, DXGI_FORMAT rtvFormat, const rhi::ResourceSlot& shaderProgram, const rhi::ResourceSlot& pso, const rhi::ResourceSlot& cb)
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

    void ImGuiViewport::wait_for_pending_operations()
    {
      m_command_queue->flush();
      ++m_fence_signaled_value;
    }

    void ImGuiViewport::begin_draw()
    {
      if (m_swapchain)
      {
        m_command_list->start_recording_commands(current_frame_ctx()->allocator());
        m_swapchain->transition_backbuffer(m_command_list->get(), D3D12_RESOURCE_STATE_RENDER_TARGET);
        m_command_list->get()->OMSetRenderTargets(1, &m_swapchain->backbuffer_view().get(), FALSE, nullptr);
      }
    }
    void ImGuiViewport::end_draw()
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

    void ImGuiViewport::clear_render_target(const ImVec4& clearColor)
    {
      m_command_list->get()->ClearRenderTargetView(m_swapchain->backbuffer_view().get(), (f32*)&clearColor, 0, nullptr);
    }
    void ImGuiViewport::draw(ImDrawData* drawData, rex::rhi::DescriptorHeap* srvDescHeap)
    {
      if (m_command_list)
      {
        ID3D12DescriptorHeap* desc_heap = srvDescHeap->get();
        m_command_list->get()->SetDescriptorHeaps(1, &desc_heap);
      }
      render_draw_data();
    }

    void ImGuiViewport::resize_buffers(s32 width, s32 height)
    {
      m_swapchain->resize_buffers(width, height, (DXGI_SWAP_CHAIN_FLAG)0);

    }
    void ImGuiViewport::present()
    {
      m_swapchain->get()->Present(0, 0);
    }
    void ImGuiViewport::yield_thread()
    {
      while (m_command_queue->fence_value() < m_fence_signaled_value)
        ::SwitchToThread();
    }

    Error ImGuiViewport::init_command_queue(ID3D12Device1* device)
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
    Error ImGuiViewport::init_frame_contexts(ID3D12Device1* device)
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
    Error ImGuiViewport::init_cmd_list(ID3D12Device1* device)
    {
      // Create command list.
      rex::wrl::ComPtr<ID3D12GraphicsCommandList> cmd_list;
      DX_CALL(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_frame_ctx[0]->allocator()->get(), nullptr, IID_PPV_ARGS(cmd_list.GetAddressOf())));
      m_command_list = rsl::make_unique<rex::rhi::CommandList2>(cmd_list);

      return Error::no_error();
    }
    Error ImGuiViewport::init_desc_heap(ID3D12Device1* device)
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
    Error ImGuiViewport::init_swapchain(::ImGuiViewport* viewport, ID3D12Device1* device, HWND hwnd, DXGI_FORMAT rtvFormat)
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

    void ImGuiViewport::update_to_next_frame_ctx()
    {
      ++m_frame_idx;
      if (m_frame_idx == m_max_num_frames_in_flight)
      {
        m_frame_idx = 0;
      }
    }
    ImGuiFrameContext* ImGuiViewport::current_frame_ctx()
    {
      return m_frame_ctx[m_frame_idx].get();
    }
    void ImGuiViewport::setup_render_state(ImDrawData* drawData, ID3D12GraphicsCommandList* ctx, class ImGuiRenderBuffer* fr)
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

    void ImGuiViewport::render_draw_data()
    {
      ImDrawData* drawData = m_imgui_viewport->DrawData;

      // Avoid rendering when minimized
      if (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f)
        return;

      if (drawData->CmdListsCount == 0)
        return;

      ImGuiViewport* vd = this;
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
  }
}