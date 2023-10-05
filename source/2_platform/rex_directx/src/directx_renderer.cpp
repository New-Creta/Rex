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

        u32 rtv_desc_size         = 0;
        u32 dsv_desc_size         = 0;
        u32 cbv_srv_uav_desc_size = 0;

        DXGI_FORMAT back_buffer_format = DXGI_FORMAT_R8G8B8A8_UNORM;

        u32 msaa_quality = 0;

        wrl::com_ptr<ID3D12CommandQueue> command_queue         = nullptr;
        wrl::com_ptr<ID3D12GraphicsCommandList> command_list   = nullptr;
        wrl::com_ptr<ID3D12CommandAllocator> command_allocator = nullptr;

        wrl::com_ptr<IDXGISwapChain> swapchain = nullptr;
        u32 current_back_buffer                = 0;

        wrl::com_ptr<ID3D12DescriptorHeap> rtv_heap;
        wrl::com_ptr<ID3D12DescriptorHeap> dsv_heap;

        wrl::com_ptr<ID3D12Resource> rtv_buffers[s_swapchain_buffer_count];
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

        if(FAILED(g_ctx.device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_ctx.command_allocator.Get() // Associated command allocator
                                                  ,
                                                  nullptr // Initial pipeline state object
                                                  ,
                                                  IID_PPV_ARGS(g_ctx.command_list.GetAddressOf()))))
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
        DXGI_SWAP_CHAIN_DESC sd               = {};
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
        sd.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;
        sd.Flags                              = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        // Note: swap chain uses queue to perform flush.
        rex::wrl::com_ptr<IDXGIFactory6> factory_6 = factory->as<IDXGIFactory6>();
        if(FAILED(factory_6->CreateSwapChain(g_ctx.command_queue.Get(), &sd, g_ctx.swapchain.GetAddressOf())))
        {
          REX_ERROR(LogDirectX, "Failed to create swap chain");
          return false;
        }

        return true;
      }

      //-------------------------------------------------------------------------
      bool create_rtvs_and_dsv_descriptor_heaps()
      {
        D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
        rtv_heap_desc.NumDescriptors             = s_swapchain_buffer_count;
        rtv_heap_desc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtv_heap_desc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        rtv_heap_desc.NodeMask                   = 0;

        if(FAILED(g_ctx.device->CreateDescriptorHeap(&rtv_heap_desc, IID_PPV_ARGS(g_ctx.rtv_heap.GetAddressOf()))))
        {
          REX_ERROR(LogDirectX, "Failed to create RTV heap");
          return false;
        }

        D3D12_DESCRIPTOR_HEAP_DESC dsv_heap_desc = {};
        rtv_heap_desc.NumDescriptors             = 1;
        rtv_heap_desc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        rtv_heap_desc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        rtv_heap_desc.NodeMask                   = 0;

        if(FAILED(g_ctx.device->CreateDescriptorHeap(&dsv_heap_desc, IID_PPV_ARGS(g_ctx.dsv_heap.GetAddressOf()))))
        {
          REX_ERROR(LogDirectX, "Failed to create DSV heap");
          return false;
        }

        return true;
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
      bool initialize(const RendererOutputWindowUserData& userData)
      {
#if defined(REX_DEBUG)
        // Enable extra debuggin and send debug messages to the VC++ output window
        rex::wrl::com_ptr<ID3D12Debug> debug_controller;
        if(FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller))))
          ;
        {
          REX_ERROR(LogDirectX, "Failed to create DX debug controller");
          return false;
        }
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
        if(create_swapchain_object() == false)
        {
          REX_ERROR(LogDirectX, "Failed to create swapchain object");
          return false;
        }

        // Create descriptor heaps for storing the rtv and dsv.
        // We need "swapchain_buffer_count" amount of rtvs to describe the buffer resources within the swapchain
        //  and one dsv to describe the depth/stencil buffer resource for depth testing.
        if(create_rtvs_and_dsv_descriptor_heaps())
        {
          REX_ERROR(LogDirectX, "Failed to create descriptor heap objects");
          return false;
        }

        // Create rtvs for each render target within the swapchain

        return true;
      }

      //-------------------------------------------------------------------------
      void shutdown()
      {
        // Nothing to implement
      }

      //-------------------------------------------------------------------------
      void new_frame()
      {
        // Nothing to implement
      }

      //-------------------------------------------------------------------------
      void end_frame()
      {
        // Nothing to implement
      }

      //-------------------------------------------------------------------------
      void clear()
      {
        // Nothing to implement
      }

      //-------------------------------------------------------------------------
      void present()
      {
        // Nothing to implement
      }
    } // namespace backend
  }   // namespace renderer
} // namespace rex