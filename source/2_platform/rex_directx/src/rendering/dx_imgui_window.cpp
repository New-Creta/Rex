#include "rex_directx/rendering/dx_imgui_window.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_directx/rendering/dx_imgui_viewport.h"
#include "rex_directx/resources/dx_clear_state_resource.h"

#include "rex_directx/system/dx_rhi.h"

namespace rex
{
  namespace renderer
  {
    ImGuiWindow::ImGuiWindow(ImGuiViewport* viewport, ID3D12Device1* device, s32 maxNumFramesInFlight, DXGI_FORMAT rtvFormat, rhi::RootSignature* rootSignature, rhi::PipelineState* pso, rhi::ConstantBuffer* cb)
      : m_viewport(viewport, device, maxNumFramesInFlight, rtvFormat, rootSignature, pso, cb)
    {
      // PlatformHandleRaw should always be a HWND, whereas PlatformHandle might be a higher-level handle (e.g. GLFWWindow*, SDL_Window*).
      // Some backends will leave PlatformHandleRaw == 0, in which case we assume PlatformHandle will contain the HWND.
      HWND hwnd = viewport->PlatformHandleRaw ? (HWND)viewport->PlatformHandleRaw : (HWND)viewport->PlatformHandle;
      IM_ASSERT(hwnd != 0);

      const s32 width = viewport->Size.x;
      const s32 height = viewport->Size.y;

      init_command_queue(device);
      init_desc_heap(device, maxNumFramesInFlight);
      init_cmd_list(device);
      init_swapchain(device, width, height, maxNumFramesInFlight, hwnd, rtvFormat);

      m_render_targets = rsl::make_unique<rsl::unique_ptr<rhi::RenderTarget>[]>(maxNumFramesInFlight);
      for (s32 i = 0; i < maxNumFramesInFlight; ++i)
      {
        rhi::Resource2* buffer = m_swapchain->get_buffer(i);
        m_render_targets[i] = rhi::create_render_target_from_backbuffer(buffer);
      }

      rhi::ClearStateDesc desc{};
      desc.rgba = rsl::colors::LightSteelBlue;
      desc.flags.add_state(ClearBits::ClearColorBuffer);

      m_clear_state = rsl::make_unique<rhi::ClearStateResource>(desc);
    }

    void ImGuiWindow::begin_draw(rex::rhi::DescriptorHeap* srvDescHeap)
    {
      rhi::RenderTarget* render_target = m_render_targets[m_swapchain->get()->GetCurrentBackBufferIndex()].get();

      m_command_list->start_recording_commands(m_viewport.current_frame_ctx()->allocator());
      m_command_list->transition_buffer(render_target, ResourceState::RenderTarget);
      m_command_list->set_render_target(render_target);



      //m_swapchain->transition_backbuffer(m_command_list->get(), D3D12_RESOURCE_STATE_RENDER_TARGET);
      //m_command_list->get()->OMSetRenderTargets(1, &m_swapchain->backbuffer_view().get(), FALSE, nullptr);
      //ID3D12DescriptorHeap* desc_heap = srvDescHeap->get();
      //m_command_list->get()->SetDescriptorHeaps(1, &desc_heap);
    }
    void ImGuiWindow::draw()
    {
      m_viewport.draw(m_command_list.get());
    }
    void ImGuiWindow::end_draw()
    {
      rhi::RenderTarget* render_target = m_render_targets[m_swapchain->get()->GetCurrentBackBufferIndex()].get();
      
      m_command_list->transition_buffer(render_target, ResourceState::Present);
      //m_swapchain->transition_backbuffer(m_command_list->get(), D3D12_RESOURCE_STATE_PRESENT);
      m_command_list->stop_recording_commands();

      m_command_queue->wait();
      m_command_queue->execute(m_command_list->dx_object());
      m_command_queue->flush();
    }

    void ImGuiWindow::clear_render_target(const ImVec4& clearColor)
    {
      rhi::RenderTarget* render_target = m_render_targets[m_swapchain->get()->GetCurrentBackBufferIndex()].get();
      m_command_list->clear_render_target(render_target, m_clear_state.get());
      //m_command_list->get()->ClearRenderTargetView(m_swapchain->backbuffer_view().get(), (f32*)&clearColor, 0, nullptr);
    }

    void ImGuiWindow::wait_for_pending_operations()
    {
      m_command_queue->flush();
    }
    void ImGuiWindow::resize_buffers(s32 width, s32 height)
    {
      m_swapchain->resize_buffers(width, height, (DXGI_SWAP_CHAIN_FLAG)0);

    }
    void ImGuiWindow::present()
    {
      m_swapchain->get()->Present(0, 0);
    }
    void ImGuiWindow::yield_thread_until_in_sync_with_gpu()
    {
      while (m_command_queue->has_reached_latest_fence())
      {
        ::SwitchToThread();
      }
    }

    Error ImGuiWindow::init_command_queue(ID3D12Device1* device)
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
    Error ImGuiWindow::init_desc_heap(ID3D12Device1* device, s32 maxNumFramesInFlight)
    {
      D3D12_DESCRIPTOR_HEAP_DESC desc = {};
      desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
      desc.NumDescriptors = maxNumFramesInFlight;
      desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
      desc.NodeMask = 1;

      rex::wrl::ComPtr<ID3D12DescriptorHeap> desc_heap;
      DX_CALL(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&desc_heap)));
      m_rtv_desc_heap = rsl::make_unique<rex::rhi::DescriptorHeap>(desc_heap, device);

      return Error::no_error();
    }
    Error ImGuiWindow::init_cmd_list(ID3D12Device1* device)
    {
      // Create command list.
      rex::wrl::ComPtr<ID3D12GraphicsCommandList> cmd_list;
      DX_CALL(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_viewport.current_frame_ctx()->allocator()->get(), nullptr, IID_PPV_ARGS(cmd_list.GetAddressOf())));
      m_command_list = rsl::make_unique<rex::rhi::CommandList>(cmd_list);

      return Error::no_error();
    }
    Error ImGuiWindow::init_swapchain(ID3D12Device1* device, s32 width, s32 height, s32 maxNumFramesInFlight, HWND hwnd, DXGI_FORMAT rtvFormat)
    {
      // Create swap chain
      // FIXME-VIEWPORT: May want to copy/inherit swap chain settings from the user/application.
      DXGI_SWAP_CHAIN_DESC1 sd1;
      ZeroMemory(&sd1, sizeof(sd1));
      sd1.BufferCount = maxNumFramesInFlight;
      sd1.Width = (UINT)width;
      sd1.Height = (UINT)height;
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

      m_swapchain = rsl::make_unique<rex::rhi::Swapchain>(d3d_swapchain_3, sd1.Format, sd1.BufferCount);

      return Error::no_error();
    }

  }
}