#include "rex_directx/d3dx12.h"
#include "rex_directx/directx_feature_level.h"
#include "rex_directx/directx_feature_shader_model.h"
#include "rex_directx/directx_util.h" // IWYU pragma: keep
#include "rex_directx/dxgi/adapter.h"
#include "rex_directx/dxgi/adapter_manager.h"
#include "rex_directx/dxgi/factory.h"
#include "rex_directx/dxgi/util.h"
#include "rex_directx/log.h"
#include "rex_directx/wrl/wrl_types.h"
#include "rex_directx/directx_vertex.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_renderer_core/gpu_description.h"
#include "rex_renderer_core/renderer.h"
#include "rex_std/algorithm.h"
#include "rex_std/bonus/string.h"
#include "rex_std/memory.h"
#include "rex_std/vector.h"
#include "rex_std_extra/memory/memory_size.h"

#include <Windows.h>
#include <cstddef>
#include <d3d12.h>
#include <DirectXColors.h>

namespace rex
{
  namespace renderer
  {
    namespace internal
    {
      //-------------------------------------------------------------------------
      count_t highest_scoring_gpu(const rsl::vector<GpuDescription>& gpus)
      {
        auto it = rsl::max_element(gpus.cbegin(), gpus.cend(),
                                   [](const GpuDescription& lhs, const GpuDescription& rhs)
                                   {
                                     const size_t lhs_vram = lhs.dedicated_video_memory.size_in_bytes();
                                     const size_t rhs_vram = rhs.dedicated_video_memory.size_in_bytes();

                                     return rhs_vram > lhs_vram;
                                   });

        return it != gpus.cend() ? rsl::distance(gpus.cbegin(), it) : -1;
      }
    } // namespace internal

    namespace directx
    {
      RendererInfo g_renderer_info; // NOLINT (fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)
    }                               // namespace directx

    //-------------------------------------------------------------------------
    const RendererInfo& info()
    {
      return directx::g_renderer_info;
    }

    //-------------------------------------------------------------------------
    ShaderPlatform shader_platform()
    {
      return ShaderPlatform::HLSL;
    }

    //-------------------------------------------------------------------------
    bool is_y_up()
    {
      return false;
    }

    //-------------------------------------------------------------------------
    bool is_depth_0_to_1()
    {
      return true;
    }

    namespace backend
    {
      static const u32 s_swapchain_buffer_count = 2;

      struct DirectXContext
      {
        wrl::com_ptr<ID3D12Device> device = nullptr;
        wrl::com_ptr<ID3D12Fence> fence   = nullptr;

        u64 current_fence;

        u32 rtv_desc_size         = 0;
        u32 dsv_desc_size         = 0;
        u32 cbv_srv_uav_desc_size = 0;

        DXGI_FORMAT back_buffer_format = DXGI_FORMAT_R8G8B8A8_UNORM;

        bool msaa_state = false;
        u32 msaa_quality = 0;

        wrl::com_ptr<ID3D12CommandQueue> command_queue         = nullptr;
        wrl::com_ptr<ID3D12GraphicsCommandList> command_list   = nullptr;
        wrl::com_ptr<ID3D12CommandAllocator> command_allocator = nullptr;

        wrl::com_ptr<IDXGISwapChain> swapchain = nullptr;
        u32 current_back_buffer = 0;

        wrl::com_ptr<ID3D12DescriptorHeap> rtv_heap = nullptr;
        wrl::com_ptr<ID3D12DescriptorHeap> dsv_heap = nullptr;

        wrl::com_ptr<ID3D12Resource> rtv_buffers[s_swapchain_buffer_count];

        DXGI_FORMAT depth_stencil_format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        wrl::com_ptr<ID3D12Resource> depth_stencil_buffer = nullptr;

        D3D12_VIEWPORT screen_viewport = {};
        RECT scissor_rect = {};
      };

      DirectXContext g_ctx; // NOLINT(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)

      //-------------------------------------------------------------------------
      bool create_command_objects()
      {
        D3D12_COMMAND_QUEUE_DESC queue_desc = {};
        queue_desc.Type                     = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queue_desc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
        if(FAILED(g_ctx.device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(g_ctx.command_queue.GetAddressOf()))))
        {
          REX_ERROR(LogDirectX, "Failed to create command queue");
          return false;
        }

        if(FAILED(g_ctx.device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(g_ctx.command_allocator.GetAddressOf()))))
        {
          REX_ERROR(LogDirectX, "Failed to create command allocator");
          return false;
        }

        if(FAILED(g_ctx.device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_ctx.command_allocator.Get()
                                                  ,nullptr
                                                  ,IID_PPV_ARGS(g_ctx.command_list.GetAddressOf()))))
        {
          REX_ERROR(LogDirectX, "Failed to create command list");
          return false;
        }

        // Start off in a closed state. This is because the first time we
        // refer to the command list we will Reset it, and it needs to be closed
        // before calling Reset.
        g_ctx.command_list->Close();

        return true;
      }

      //-------------------------------------------------------------------------
      bool create_swapchain_object(dxgi::Factory* factory, const RendererOutputWindowUserData& userData)
      {
        DXGI_SWAP_CHAIN_DESC sd;
        sd.BufferDesc.Width                   = userData.window_width;
        sd.BufferDesc.Height                  = userData.window_height;
        sd.BufferDesc.RefreshRate.Numerator   = userData.refresh_rate;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferDesc.Format                  = g_ctx.back_buffer_format;
        sd.BufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        sd.BufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;
        sd.SampleDesc.Count                   = g_ctx.msaa_state ? 4 : 1;
        sd.SampleDesc.Quality                 = g_ctx.msaa_state ? g_ctx.msaa_quality - 1 : 0;
        sd.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount                        = s_swapchain_buffer_count;
        sd.OutputWindow                       = (HWND)userData.primary_display_handle;
        sd.Windowed                           = userData.windowed;
        sd.SwapEffect                         = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd.Flags                              = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        // Note: swap chain uses queue to perform flush.
        rex::wrl::com_ptr<IDXGIFactory> dxgi_factory = factory->as<IDXGIFactory>();
        if(FAILED(dxgi_factory->CreateSwapChain(g_ctx.command_queue.Get(), &sd, g_ctx.swapchain.GetAddressOf())))
        {
          REX_ERROR(LogDirectX, "Failed to create swap chain");
          return false;
        }

        return true;
      }

      //-------------------------------------------------------------------------
      bool create_rtvs_and_dsv_descriptor_heaps()
      {
        D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc;
        rtv_heap_desc.NumDescriptors             = s_swapchain_buffer_count;
        rtv_heap_desc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtv_heap_desc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        rtv_heap_desc.NodeMask                   = 0;

        if(FAILED(g_ctx.device->CreateDescriptorHeap(&rtv_heap_desc, IID_PPV_ARGS(g_ctx.rtv_heap.GetAddressOf()))))
        {
          REX_ERROR(LogDirectX, "Failed to create RTV heap");
          return false;
        }

        D3D12_DESCRIPTOR_HEAP_DESC dsv_heap_desc;
        dsv_heap_desc.NumDescriptors             = 1;
        dsv_heap_desc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsv_heap_desc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        dsv_heap_desc.NodeMask                   = 0;

        if(FAILED(g_ctx.device->CreateDescriptorHeap(&dsv_heap_desc, IID_PPV_ARGS(g_ctx.dsv_heap.GetAddressOf()))))
        {
          REX_ERROR(LogDirectX, "Failed to create DSV heap");
          return false;
        }

        return true;
      }

      //-------------------------------------------------------------------------
      ID3D12Resource* get_current_backbuffer()
      {
        return g_ctx.rtv_buffers[g_ctx.current_back_buffer].Get();
      }

      //-------------------------------------------------------------------------
      D3D12_CPU_DESCRIPTOR_HANDLE get_current_backbuffer_descriptor()
      {
        return CD3DX12_CPU_DESCRIPTOR_HANDLE(g_ctx.rtv_heap->GetCPUDescriptorHandleForHeapStart(), g_ctx.current_back_buffer, g_ctx.rtv_desc_size);
      }
      //-------------------------------------------------------------------------
      D3D12_CPU_DESCRIPTOR_HANDLE get_depthstencil_descriptor()
      {
        return g_ctx.dsv_heap->GetCPUDescriptorHandleForHeapStart();
      }

      //-------------------------------------------------------------------------
      bool create_rtvs_for_swapchain()
      {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(g_ctx.rtv_heap->GetCPUDescriptorHandleForHeapStart());
        for(u32 i = 0; i < s_swapchain_buffer_count; ++i)
        {
          if(FAILED(g_ctx.swapchain->GetBuffer(i, IID_PPV_ARGS(g_ctx.rtv_buffers[i].GetAddressOf()))))
          {
            REX_ERROR(LogDirectX, "Failed to retrieve swapchain buffer");
            return false;
          }

          g_ctx.device->CreateRenderTargetView(g_ctx.rtv_buffers[i].Get(), nullptr, rtv_handle);

          rtv_handle.Offset(1, g_ctx.rtv_desc_size);
        }

        return true;
      }

      //-------------------------------------------------------------------------
      bool create_dsv(const RendererOutputWindowUserData& userData)
      {
        D3D12_RESOURCE_DESC depth_stencil_desc = {};

        depth_stencil_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        depth_stencil_desc.Alignment = 0;
        depth_stencil_desc.Width = userData.window_width;
        depth_stencil_desc.Height = userData.window_height;
        depth_stencil_desc.DepthOrArraySize = 1;
        depth_stencil_desc.MipLevels = 1;
        depth_stencil_desc.Format = g_ctx.depth_stencil_format;
        depth_stencil_desc.SampleDesc.Count = g_ctx.msaa_state ? 4 : 1;
        depth_stencil_desc.SampleDesc.Quality = g_ctx.msaa_state ? g_ctx.msaa_quality - 1 : 0;
        depth_stencil_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        depth_stencil_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        D3D12_CLEAR_VALUE optimized_clear_value = {};
        optimized_clear_value.Format = g_ctx.depth_stencil_format;
        optimized_clear_value.DepthStencil.Depth = 1.0f;
        optimized_clear_value.DepthStencil.Stencil = 0;
        
        CD3DX12_HEAP_PROPERTIES heap_properties(D3D12_HEAP_TYPE_DEFAULT);

        if(FAILED(g_ctx.device->CreateCommittedResource(&heap_properties
            , D3D12_HEAP_FLAG_NONE
            , &depth_stencil_desc
            , D3D12_RESOURCE_STATE_COMMON
            , &optimized_clear_value
            , IID_PPV_ARGS(g_ctx.depth_stencil_buffer.GetAddressOf()))))
        {
          REX_ERROR(LogDirectX, "Failed to create depth stencil buffer");
          return false;
        }

        // Create descriptor to mip level 0 of entire resource using the 
        // format of the resource
        D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc;
        dsv_desc.Flags = D3D12_DSV_FLAG_NONE;
        dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        dsv_desc.Format = g_ctx.depth_stencil_format;
        dsv_desc.Texture2D.MipSlice = 0;

        g_ctx.device->CreateDepthStencilView(g_ctx.depth_stencil_buffer.Get(), &dsv_desc, get_depthstencil_descriptor());

        // Transition the resouce from it's inital state to be used as a depth buffer
        CD3DX12_RESOURCE_BARRIER depth_write_transition = CD3DX12_RESOURCE_BARRIER::Transition(g_ctx.depth_stencil_buffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
        g_ctx.command_list->ResourceBarrier(1, &depth_write_transition);

        return true;
      }

      //-------------------------------------------------------------------------
      bool flush_command_queue()
      {
        // Advance the fence value to mark commands up to this fence point.
        g_ctx.current_fence++;

        // Add an instruction to the command queue to set a new fence point. Because we
        // are on the GPU timeline, the new fence point won't be set until the GPU finishes
        // processing all the commands prior to this Signal().
        if (FAILED(g_ctx.command_queue->Signal(g_ctx.fence.Get(), g_ctx.current_fence)))
        {
          REX_ERROR(LogDirectX, "Failed to signal command queue with fence");
          return false;
        }

        if (g_ctx.fence->GetCompletedValue() < g_ctx.current_fence)
        {
          HANDLE event_handle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

          //Fire event when GPU hits current fence
          if (FAILED(g_ctx.fence->SetEventOnCompletion(g_ctx.current_fence, event_handle)))
          {
            REX_ERROR(LogDirectX, "Failed to set completion event for fence");
            return false;
          }

          WaitForSingleObject(event_handle, INFINITE);
          CloseHandle(event_handle);
        }

        return true;
      }

      //-------------------------------------------------------------------------
      bool initialize(const RendererOutputWindowUserData& userData)
      {
#if defined(REX_DEBUG)
        // Enable extra debuggin and send debug messages to the VC++ output window
        rex::wrl::com_ptr<ID3D12Debug> debug_controller;
        if(FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller))))
        {
          REX_ERROR(LogDirectX, "Failed to create DX debug controller");
          return false;
        }

        debug_controller->EnableDebugLayer();
#endif

        dxgi::Factory factory = dxgi::Factory::create();
        if(!factory)
        {
          REX_ERROR(LogDirectX, "Failed to create DXGI Factory");
          return false;
        }

        const dxgi::AdapterManager adapter_manager(&factory, &internal::highest_scoring_gpu);
        const dxgi::Adapter* selected_gpu = adapter_manager.selected();
        IDXGIAdapter* adapter             = selected_gpu->c_ptr();

        const D3D_FEATURE_LEVEL feature_level = query_feature_level(adapter);

        if(FAILED(D3D12CreateDevice(adapter, static_cast<D3D_FEATURE_LEVEL>(feature_level), IID_PPV_ARGS(&g_ctx.device))))
        {
          REX_ERROR(LogDirectX, "Software adapter not supported");
          REX_ERROR(LogDirectX, "Failed to create DX12 Device");
          return false;
        }

        REX_LOG(LogDirectX, "D3D12 Device Created!");

        const D3D_SHADER_MODEL shader_model = query_shader_model_version(g_ctx.device.Get());

        directx::g_renderer_info.shader_version = rsl::string(shader_model_name(shader_model));
        directx::g_renderer_info.api_version    = rsl::string(feature_level_name(feature_level));
        directx::g_renderer_info.adaptor        = selected_gpu->description().name;
        directx::g_renderer_info.vendor         = selected_gpu->description().vendor_name;

        // Create fence for CPU/GPU synchronization
        if(FAILED(g_ctx.device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_ctx.fence))))
        {
          REX_ERROR(LogDirectX, "Failed to create DX fence, to synchronize CPU/GPU");
          return false;
        }

        // Descriptor sizes vary across GPU so we need to query this information
        g_ctx.rtv_desc_size         = g_ctx.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        g_ctx.dsv_desc_size         = g_ctx.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        g_ctx.cbv_srv_uav_desc_size = g_ctx.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        // 4x MSAA is supported on all DX11 hardware, since we only support DX12 devices we are guaranteed that 4x MSAA is supported.
        // However, we do have to check the supported quality level.
        D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS ms_quality_levels;
        ms_quality_levels.Format           = g_ctx.back_buffer_format;
        ms_quality_levels.SampleCount      = 4;
        ms_quality_levels.Flags            = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
        ms_quality_levels.NumQualityLevels = 0;
        if(FAILED(g_ctx.device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &ms_quality_levels, sizeof(ms_quality_levels))))
        {
          REX_ERROR(LogDirectX, "Given multisample quality level is not supported by this hardware");
          return false;
        }

        g_ctx.msaa_quality = ms_quality_levels.NumQualityLevels;

        REX_ASSERT_X(g_ctx.msaa_quality > 0, "Unexcpected MSAA quality level");

        // Create command queue, command allocator and command list objects
        if(create_command_objects() == false)
        {
          REX_ERROR(LogDirectX, "Failed to create command objects");
          return false;
        }

        // Create swapchain object
        if(create_swapchain_object(&factory, userData) == false)
        {
          REX_ERROR(LogDirectX, "Failed to create swapchain object");
          return false;
        }

        // Create descriptor heaps for storing the rtv and dsv.
        // We need "swapchain_buffer_count" amount of rtvs to describe the buffer resources within the swapchain
        //  and one dsv to describe the depth/stencil buffer resource for depth testing.
        if(create_rtvs_and_dsv_descriptor_heaps() == false)
        {
          REX_ERROR(LogDirectX, "Failed to create descriptor heap objects");
          return false;
        }

        // Create rtvs for each render target within the swapchain
        if(create_rtvs_for_swapchain() == false)
        {
          REX_ERROR(LogDirectX, "Failed to create rtvs for swapchain");
          return false;
        }

        // Create depth/stencil view to bind to the pipeline and
        // depth/stencil buffer as resource to write depth values to
        if (create_dsv(userData) == false)
        {
          REX_ERROR(LogDirectX, "Failed to create dsv");
          return false;
        }

        // Create viewport to render our image in
        // A viewport always needs to reset whenever a command list is reset
        g_ctx.screen_viewport.TopLeftX = 0.0f;
        g_ctx.screen_viewport.TopLeftY = 0.0f;
        g_ctx.screen_viewport.Width = static_cast<f32>(userData.window_width);
        g_ctx.screen_viewport.Height = static_cast<f32>(userData.window_height);
        g_ctx.screen_viewport.MinDepth = 0.0f;
        g_ctx.screen_viewport.MaxDepth = 0.0f;

        g_ctx.command_list->RSSetViewports(1, &g_ctx.screen_viewport);
        
        // Cull pixels drawn outside of the backbuffer ( such as UI elements )
        g_ctx.scissor_rect = {0, 0, static_cast<s32>(userData.window_width * 0.5f), static_cast<s32>(userData.window_height * 0.5f)};

        g_ctx.command_list->RSSetScissorRects(1, &g_ctx.scissor_rect);

        return true;
      }

      //-------------------------------------------------------------------------
      void shutdown()
      {
        // Nothing to implement
      }

      //-------------------------------------------------------------------------
      bool new_frame()
      {
        // Reuse the memory assosiated with command recording.
        // We can only reset when the associated command lists have finished
        // execution on the GPU.
        if (FAILED(g_ctx.command_allocator->Reset()))
        {
          REX_ERROR(LogDirectX, "Failed to reset command allocator");
          return false;
        }

        // a command list can be reset after it has been added to the command queue via ExecuteCommandList. Reusing the 
        // command list reuses memory.
        if (FAILED(g_ctx.command_list->Reset(g_ctx.command_allocator.Get(), nullptr)))
        {
          REX_ERROR(LogDirectX, "Failed to reset command list");
          return false;
        }

        // Indicate a state transition on the resouce usage.
        D3D12_RESOURCE_BARRIER render_target_transition = CD3DX12_RESOURCE_BARRIER::Transition(get_current_backbuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

        g_ctx.command_list->ResourceBarrier(1, &render_target_transition);

        // Set the viewport and scissor rect. This needs to be reset
        // whenever the command list is reset.
        g_ctx.command_list->RSSetViewports(1, &g_ctx.screen_viewport);
        g_ctx.command_list->RSSetScissorRects(1, &g_ctx.scissor_rect);

        return true;
      }

      //-------------------------------------------------------------------------
      bool end_frame()
      {
        // Wait until frame commands are complete. This waiting is 
        // inefficient and is done for simplicity.
        flush_command_queue();

        return true;
      }

      //-------------------------------------------------------------------------
      void clear()
      {
        D3D12_CPU_DESCRIPTOR_HANDLE backbuffer_desc   = get_current_backbuffer_descriptor();
        D3D12_CPU_DESCRIPTOR_HANDLE depthstencil_desc = get_depthstencil_descriptor();

        // Clear the back buffer and depth buffer
        g_ctx.command_list->ClearRenderTargetView(backbuffer_desc, DirectX::Colors::LightSteelBlue, 0, nullptr);
        g_ctx.command_list->ClearDepthStencilView(depthstencil_desc, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

        // Specify the buffers we are going to render towards.
        g_ctx.command_list->OMSetRenderTargets(1, &backbuffer_desc, true, &depthstencil_desc);
      }

      //-------------------------------------------------------------------------
      bool present()
      {
        // Indicate a state transition on the resouce usage.
        D3D12_RESOURCE_BARRIER present_transition = CD3DX12_RESOURCE_BARRIER::Transition(get_current_backbuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        g_ctx.command_list->ResourceBarrier(1, &present_transition);

        // Done recording commands!
        if(FAILED(g_ctx.command_list->Close()))
        {
          REX_ERROR(LogDirectX, "Failed to close command list");
          return false;
        }

        // Add the command list to the queue for execution
        ID3D12CommandList* command_lists[] = {g_ctx.command_list.Get()};
        g_ctx.command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);

        // Swap the back and front buffers
        if(FAILED(g_ctx.swapchain->Present(0, 0)))
        {
          REX_ERROR(LogDirectX, "Failed to present swapchain");
          return false;
        }

        g_ctx.current_back_buffer = (g_ctx.current_back_buffer + 1) % s_swapchain_buffer_count;

        return true;
      }
    } // namespace backend
  }   // namespace renderer
} // namespace rex