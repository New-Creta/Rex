#include "rex_directx/dxgi/rhi.h"

#include "rex_directx/dxgi/factory.h"
#include "rex_directx/dxgi/adapter_manager.h"
#include "rex_directx/dxgi/adapter.h"

#include "rex_directx/utility/directx_util.h"

#include "rex_directx/diagnostics/directx_call.h"
#include "rex_directx/diagnostics/log.h"
#include "rex_directx/system/directx_device.h"
#include "rex_directx/system/directx_feature_level.h"
#include "rex_directx/system/swapchain.h"

#include "rex_engine/engine/types.h"
#include "rex_engine/diagnostics/assert.h"

#include "rex_renderer_core/rendering/renderer_output_window_user_data.h"

#include "rex_std/bonus/utility.h"

namespace rex
{
  namespace rhi
  {
    DEFINE_LOG_CATEGORY(LogRhi, rex::LogVerbosity::Log);

    namespace internal
    {
      rsl::unique_ptr<dxgi::Factory> g_factory;
      rsl::unique_ptr<renderer::DirectXDevice> g_device;
      rsl::unique_ptr<renderer::Swapchain> g_swapchain;
      wrl::ComPtr<ID3D12CommandQueue> g_command_queue;
      wrl::ComPtr<ID3D12CommandAllocator> g_command_allocator;
      wrl::ComPtr<ID3D12GraphicsCommandList> g_command_list;
      static constexpr s32 s_swapchain_buffer_count = 2;

      // DXGI Factory
      s32 init_debug_interface()
      {
        /*
        * Bug in the DXGI Debug Layer interaction with the DX12 Debug Layer w/ Windows 11.
        * There's a simple workaround which is to suppress D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE.
        * The bug itself will be fixed in a future Windows update.
        *
        * The Debug Layer has always had quirks when it comes to dealing with 'hybrid graphics' systems
        * (i.e. laptops with both Intel Integrated and discrete GPUs)
        *
        * https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
        * https://github.com/walbourn/directx-vs-templates/commit/18e3eaa444e98ba75d37d506ab18df8db0b82441
        */
        s32 dxgi_factory_flags = 0;

        wrl::ComPtr<IDXGIInfoQueue> dxgi_info_queue;
        if (DX_SUCCESS(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgi_info_queue.GetAddressOf()))))
        {
          dxgi_factory_flags = DXGI_CREATE_FACTORY_DEBUG;

          dxgi_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_MESSAGE, globals::g_enable_dxgi_severity_message);
          dxgi_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_INFO, globals::g_enable_dxgi_severity_info);
          dxgi_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING, globals::g_enable_dxgi_severity_warning);
          dxgi_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, globals::g_enable_dxgi_severity_error);
          dxgi_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, globals::g_enable_dxgi_severity_corruption);

          rsl::array<DXGI_INFO_QUEUE_MESSAGE_ID, 1> dxgi_hide = {
              80 /* IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides. */,
          };
          DXGI_INFO_QUEUE_FILTER dxgi_filter = {};
          dxgi_filter.DenyList.NumIDs = rsl::safe_numeric_cast<u32>(dxgi_hide.size());
          dxgi_filter.DenyList.pIDList = dxgi_hide.data();

          dxgi_info_queue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &dxgi_filter);

          g_ctx->dxgi_debug_layer_enabled = true;
        }
        else
        {
          REX_WARN(LogDirectX, "Unable to get GXGI Debug Interface");
        }

        return dxgi_factory_flags;
      }
      bool init_dxgi_factory()
      {
        s32 dxgi_factory_flags = 0;

#ifdef REX_ENABLE_DXGI_DEBUG_LAYER
        dxgi_factory_flags = internal::init_debug_interface();
#endif

        g_factory = dxgi::Factory::create(dxgi_factory_flags);
      }

      // D3D Device
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
      const rsl::array g_expected_feature_levels = { D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_1 };

      bool init_d3d_device()
      {
        // Find highest scoring gpu
        const dxgi::AdapterManager adapter_manager(g_factory.get(), &internal::highest_scoring_gpu);
        const dxgi::Adapter* selected_gpu = adapter_manager.selected();
        IDXGIAdapter* adapter = selected_gpu->c_ptr();

        // Create device
        const D3D_FEATURE_LEVEL feature_level = query_feature_level(adapter);

        wrl::ComPtr<ID3D12Device> device;
        if (DX_FAILED(D3D12CreateDevice(adapter, static_cast<D3D_FEATURE_LEVEL>(feature_level), IID_PPV_ARGS(&device))))
        {
          REX_ERROR(LogDirectX, "Software adapter not supported");
          REX_ERROR(LogDirectX, "Failed to create DX12 Device");
          return false;
        }

        g_device = rsl::make_unique<renderer::DirectXDevice>(device, feature_level, selected_gpu);
        return true;
      }

      // D3D Command Objects
      bool init_command_objects()
      {
        // Command Queue
        D3D12_COMMAND_QUEUE_DESC queue_desc = {};
        queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        if (DX_FAILED(g_device->get()->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(g_command_queue.GetAddressOf()))))
        {
          REX_ERROR(LogDirectX, "Failed to create command queue");
          return false;
        }

        rhi::set_debug_name_for(g_command_queue.Get(), "Global Command Queue");

        // Command Allocator
        if (DX_FAILED(g_device->get()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(g_command_allocator.GetAddressOf()))))
        {
          REX_ERROR(LogDirectX, "Failed to create command allocator");
          return false;
        }

        rhi::set_debug_name_for(g_command_allocator.Get(), "Global Command Allocator");

        // Command List
        if (DX_FAILED(g_device->get()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_command_allocator.Get(), nullptr, IID_PPV_ARGS(g_command_list.GetAddressOf()))))
        {
          REX_ERROR(LogDirectX, "Failed to create command list");
          return false;
        }

        rhi::set_debug_name_for(g_command_list.Get(), "Global Command List");

        // Start off in a closed state. This is because the first time we
        // refer to the command list we will Reset it, and it needs to be closed
        // before calling Reset.
        if (DX_FAILED(g_command_list->Close()))
        {
          REX_ERROR(LogDirectX, "Failed to close command list");
          return false;
        }

        return true;
      }
      
      // DXGI Swapchain
      bool init_swapchain(const renderer::OutputWindowUserData& userData)
      {
        DXGI_SWAP_CHAIN_DESC sd;
        sd.BufferDesc.Width = userData.window_width;
        sd.BufferDesc.Height = userData.window_height;
        sd.BufferDesc.RefreshRate.Numerator = userData.refresh_rate;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = s_swapchain_buffer_count;
        sd.OutputWindow = (HWND)userData.primary_display_handle;
        sd.Windowed = userData.windowed;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        // Note: swap chain uses queue to perform flush.
        rex::wrl::ComPtr<IDXGIFactory> dxgi_factory = g_factory->as<IDXGIFactory>();
        rex::wrl::ComPtr<IDXGISwapChain> swapchain;
        if (DX_FAILED(dxgi_factory->CreateSwapChain(g_command_queue.Get(), &sd, swapchain.GetAddressOf())))
        {
          REX_ERROR(LogDirectX, "Failed to create swap chain");
          return false;
        }

        rhi::set_debug_name_for(swapchain.Get(), "SwapChain");

        g_swapchain = rsl::make_unique<renderer::Swapchain>(swapchain);
      }

      // Resource Heaps
      bool init_resource_heaps()
      {

      }

      // Descriptor Heaps
      bool init_descriptor_heaps()
      {
        rsl::array<D3D12_DESCRIPTOR_HEAP_DESC, 3> heap_descs;

        heap_descs[0] = {
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV, rsl::safe_numeric_cast<UINT>(numRTV), D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
            0 // For single-adapter operation, set this to zero. ( https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_descriptor_heap_desc )
        };
        heap_descs[1] = {
            D3D12_DESCRIPTOR_HEAP_TYPE_DSV, rsl::safe_numeric_cast<UINT>(numDSV), D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
            0 // For single-adapter operation, set this to zero. ( https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_descriptor_heap_desc )
        };
        heap_descs[2] = {
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, rsl::safe_numeric_cast<UINT>(numCBV), D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
            0 // For single-adapter operation, set this to zero. ( https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_descriptor_heap_desc )
        };

        for (s32 i = 0; i < heap_descs.size(); ++i)
        {
          auto heap_desc = &heap_descs[i];

          if (DX_FAILED(g_ctx->device->get()->CreateDescriptorHeap(heap_desc, IID_PPV_ARGS(&g_ctx->descriptor_heap_pool[heap_desc->Type]))))
          {
            REX_ERROR(LogDirectX, "Failed to create descriptor heap for type: {}", rsl::enum_refl::enum_name(heap_desc->Type));
            return false;
          }

          switch (heap_desc->Type)
          {
          case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
          {
            rhi::set_debug_name_for(g_ctx->descriptor_heap_pool[heap_desc->Type].Get(), rsl::format("Descriptor Heap Element - RTV {}", i));
            REX_LOG(LogDirectX, "Created {0} ( amount created: {1}) ", rsl::enum_refl::enum_name(heap_desc->Type), numRTV);
          }
          break;
          case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
          {
            rhi::set_debug_name_for(g_ctx->descriptor_heap_pool[heap_desc->Type].Get(), rsl::format("Descriptor Heap Element - DSV {}", i));
            REX_LOG(LogDirectX, "Created {0} ( amount created: {1}) ", rsl::enum_refl::enum_name(heap_desc->Type), numDSV);
          }
          break;
          case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
          {
            rhi::set_debug_name_for(g_ctx->descriptor_heap_pool[heap_desc->Type].Get(), rsl::format("Descriptor Heap Element - CBV {}", i));
            REX_LOG(LogDirectX, "Created {0} ( amount created: {1}) ", rsl::enum_refl::enum_name(heap_desc->Type), numCBV);
          }
          break;
          default:
            REX_ASSERT("Unknown Descriptor Heap Type");
          }
        }

        return true;
      }
    }


    bool init(const renderer::OutputWindowUserData& userData)
    {
      // 1) we need to init the dxgi factory.
      // This is the system we use to create most other systems.
      if (!internal::init_dxgi_factory())
      {
        REX_ERROR(LogRhi, "Failed to create DXGI Factory");
        return false;
      }

      // 2) we need to init the device.
      // A DirectX Device is used for creation of other resources
      // You can think of it as an abstraction of the GPU,
      // but just an abstraction over resource creation,
      // not an abstraction of the gpu itself, that's what an IDXGIAdapter is for.
      if (!internal::init_d3d_device())
      {
        REX_ERROR(LogRhi, "Failed to create D3D Device");
        return false;
      }

      // 3) we create the command objects.
      // A gpu works with commands, that get added to a command list.
      // command lists themselves are not executable, but need to be added
      // to a command queue, which in turn executes the commands.
      if (!internal::init_command_objects())
      {
        REX_ERROR(LogRhi, "Failed to create command objects");
      }

      // 4) we need to create a swapchain which is responsible of presenting.
      // There's no benefit in creating all the above systems if we don't have anything
      // to actually present something on screen, that's what the swapchain is for.
      if (!internal::init_swapchain(userData))
      {
        REX_ERROR(LogRhi, "Failed to create swapchain");
      }
      
      // 5) We need to create initial heaps that store the data that'll be used for rendering
      // These heaps only hold the data, but letting the graphics pipeline know which resource to use
      // is never done with the heaps directly, but instead are only used to store the data
      // To let graphics pipeline know which resources to use, we use descriptors, which are stored
      // in their own heap
      if (!internal::init_resource_heaps())
      {
        REX_ERROR(LogRhi, "Failed to create resource heaps");
      }

      // 6) We need a few heaps to store descriptors which point to the actual resources in other heaps
      // A descriptor is just some metadata about the resource, holding all the information
      // the gpu needs to use a resource.
      // A heap on the gpu is just the same as on the cpu except it's more limited to what it can store
      // You don't get random access into it, a heap can only store specific types and you access into the heap by offset
      // Where each offset is the multiplied by the size of the type the heap stores.
      // The heap doesn't store the content of a resource but stores the view to the resource.
      if (!internal::init_descriptor_heaps())
      {
        REX_ERROR(LogRhi, "Failed to create descriptor heaps");
      }
    }

    rex::wrl::ComPtr<IDXGISwapChain> create_swapchain(ID3D12CommandList* commandlist, DXGI_SWAP_CHAIN_DESC& desc)
    {
      rex::wrl::ComPtr<IDXGIFactory> dxgi_factory = g_factory->as<IDXGIFactory>();
      rex::wrl::ComPtr<IDXGISwapChain> swapchain;

      // Note: swap chain uses queue to perform flush.
      if (DX_FAILED(dxgi_factory->CreateSwapChain(commandlist, &desc, swapchain.GetAddressOf())))
      {
        REX_ERROR(LogDirectX, "Failed to create swap chain");
        return swapchain;
      }

      set_debug_name_for(swapchain.Get(), "SwapChain");
      return swapchain;
    }
  }
}