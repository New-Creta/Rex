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
#include "rex_directx/d3dx12.h"

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
      wrl::ComPtr<ID3D12Fence> g_fence;
      wrl::ComPtr<ID3D12Heap> g_heap;

      rsl::unordered_map<D3D12_DESCRIPTOR_HEAP_TYPE, wrl::ComPtr<ID3D12DescriptorHeap>> g_descriptor_heap_pool;

      constexpr s32 g_swapchain_buffer_count = 2;
      constexpr s32 g_num_rtv_descs = 8;
      constexpr s32 g_num_dsv_descs = 1;
      constexpr s32 g_num_cbv_descs = 128;

      rsl::array<s32, D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES> g_descriptor_desc_sizes = {};

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
        }
        else
        {
          REX_WARN(LogRhi, "Unable to get GXGI Debug Interface");
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
        return true;
      }
      void init_debug_controller()
      {
        // Enable extra debugging and send debug messages to the VC++ output window
        rex::wrl::ComPtr<ID3D12Debug> debug_controller;
        if (DX_SUCCESS(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller))))
        {
          debug_controller->EnableDebugLayer();
        }
        else
        {
          REX_WARN(LogDirectX, "Failed to create DX debug controller");
        }
      }

      // D3D Device
      const rsl::array g_expected_feature_levels = { D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_1 };
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
      void init_debug_layer()
      {
        // Device needs to exist before we can query this
        rex::wrl::ComPtr<ID3D12InfoQueue> dx12_info_queue;
        if (DX_SUCCESS(g_device->get()->QueryInterface(IID_PPV_ARGS(dx12_info_queue.GetAddressOf()))))
        {
          dx12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_MESSAGE, globals::g_enable_dx12_severity_message);
          dx12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_INFO, globals::g_enable_dx12_severity_info);
          dx12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, globals::g_enable_dx12_severity_warning);
          dx12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, globals::g_enable_dx12_severity_error);
          dx12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, globals::g_enable_dx12_severity_corruption);

          /*
           * Bug in the DX12 Debug Layer interaction with the DX12 Debug Layer w/ Windows 11.
           * There's a simple workaround which is to suppress D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE.
           * The bug itself will be fixed in a future Windows update.
           *
           * The Debug Layer has always had quirks when it comes to dealing with 'hybrid graphics' systems
           * (i.e. laptops with both Intel Integrated and discrete GPUs)
           *
           * https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
           * https://github.com/walbourn/directx-vs-templates/commit/2b34dcf9ac764153699058cdc9d4dbc4e837224c
           */
          rsl::array<D3D12_MESSAGE_ID, 7> dx12_hide = { D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
                             D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
                             D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE,
                             D3D12_MESSAGE_ID_RESOURCE_BARRIER_BEFORE_AFTER_MISMATCH,
                             D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE,
                             D3D12_MESSAGE_ID_COMMAND_LIST_DRAW_VERTEX_BUFFER_NOT_SET,
                             D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE };

          D3D12_INFO_QUEUE_FILTER dx12_filter = {};
          dx12_filter.DenyList.NumIDs = rsl::safe_numeric_cast<u32>(dx12_hide.size());
          dx12_filter.DenyList.pIDList = dx12_hide.data();
          dx12_info_queue->AddStorageFilterEntries(&dx12_filter);
        }
        else
        {
          REX_WARN(LogDirectX, "Unable to get D3D12 Debug Interface");
        }
      }

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
          REX_ERROR(LogRhi, "Software adapter not supported");
          REX_ERROR(LogRhi, "Failed to create DX12 Device");
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
          REX_ERROR(LogRhi, "Failed to create command queue");
          return false;
        }

        rhi::set_debug_name_for(g_command_queue.Get(), "Global Command Queue");

        // Command Allocator
        if (DX_FAILED(g_device->get()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(g_command_allocator.GetAddressOf()))))
        {
          REX_ERROR(LogRhi, "Failed to create command allocator");
          return false;
        }

        rhi::set_debug_name_for(g_command_allocator.Get(), "Global Command Allocator");

        // Command List
        if (DX_FAILED(g_device->get()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_command_allocator.Get(), nullptr, IID_PPV_ARGS(g_command_list.GetAddressOf()))))
        {
          REX_ERROR(LogRhi, "Failed to create command list");
          return false;
        }

        rhi::set_debug_name_for(g_command_list.Get(), "Global Command List");

        // Start off in a closed state. This is because the first time we
        // refer to the command list we will Reset it, and it needs to be closed
        // before calling Reset.
        if (DX_FAILED(g_command_list->Close()))
        {
          REX_ERROR(LogRhi, "Failed to close command list");
          return false;
        }

        return true;
      }
      
      // D3D Fence
      bool init_global_fence()
      {
        if (DX_FAILED(g_device->get()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence))))
        {
          REX_ERROR(LogDirectX, "Failed to create DX fence, to synchronize CPU/GPU");
          return false;
        }

        rhi::set_debug_name_for(g_fence.Get(), "Global Fence");
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
        sd.BufferCount = g_swapchain_buffer_count;
        sd.OutputWindow = (HWND)userData.primary_display_handle;
        sd.Windowed = userData.windowed;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        // Note: swap chain uses queue to perform flush.
        rex::wrl::ComPtr<IDXGIFactory> dxgi_factory = g_factory->as<IDXGIFactory>();
        rex::wrl::ComPtr<IDXGISwapChain> swapchain;
        if (DX_FAILED(dxgi_factory->CreateSwapChain(g_command_queue.Get(), &sd, swapchain.GetAddressOf())))
        {
          REX_ERROR(LogRhi, "Failed to create swap chain");
          return false;
        }

        rhi::set_debug_name_for(swapchain.Get(), "SwapChain");

        g_swapchain = rsl::make_unique<renderer::Swapchain>(swapchain);
        return true;
      }

      // Resource Heaps
      bool init_resource_heaps()
      {
        // Nothing to implement at the moment
        CD3DX12_HEAP_DESC desc(1_kib, D3D12_HEAP_TYPE_DEFAULT);

        if (DX_FAILED(g_device->get()->CreateHeap(&desc, IID_PPV_ARGS(&g_heap))))
        {
          REX_ERROR(LogRhi, "Failed to create global resource heap");
          return false;
        }

        return true;
      }

      // Descriptor Heaps
      bool init_descriptor_heap(D3D12_DESCRIPTOR_HEAP_TYPE type, s32 numDescriptors)
      {
        D3D12_DESCRIPTOR_HEAP_DESC desc{};

        desc.Type = type;
        desc.NumDescriptors = numDescriptors;
        desc.Flags = type != D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
          ? D3D12_DESCRIPTOR_HEAP_FLAG_NONE
          : D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        desc.NodeMask = 0; // For single-adapter operation, set this to zero. ( https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_descriptor_heap_desc )

        if (DX_FAILED(g_device->get()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_descriptor_heap_pool[desc.Type]))))
        {
          REX_ERROR(LogRhi, "Failed to create descriptor heap for type: {}", rsl::enum_refl::enum_name(desc.Type));
          return false;
        }

        s32 desc_size = g_device->get()->GetDescriptorHandleIncrementSize(type);
        s32 total_size = desc_size * numDescriptors;
        g_descriptor_desc_sizes[type] = desc_size;
        rsl::string_view type_str = rsl::enum_refl::enum_name(type);
        rhi::set_debug_name_for(g_descriptor_heap_pool[desc.Type].Get(), rsl::format("Descriptor Heap Element - {}", type_str));
        REX_LOG(LogRhi, "Created {0} ( num: {1} descriptors, desc size: {2} bytes, total size: {3} bytes) ", type_str, numDescriptors, desc_size, total_size);

        return true;
      }
      bool init_descriptor_heaps()
      {
        if (!init_descriptor_heap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, g_num_rtv_descs))
        {
          REX_ERROR(LogRhi, "Failed to create descriptor heap for RTV");
          return false;
        }
        if (!init_descriptor_heap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, g_num_dsv_descs))
        {
          REX_ERROR(LogRhi, "Failed to create descriptor heap for RTV");
          return false;
        }
        if (!init_descriptor_heap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, g_num_cbv_descs))
        {
          REX_ERROR(LogRhi, "Failed to create descriptor heap for RTV");
          return false;
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

#ifdef REX_ENABLE_DX12_DEBUG_LAYER
      // 1.1) Create the debug controller before the device gets created
      // This way we can have some additional debugging information if something goes wrong
      internal::init_debug_controller();
#endif

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

#ifdef REX_ENABLE_DX12_DEBUG_LAYER
      // 2.1) Create the debug layer aftet device gets created
      // The debug layer is linked to the device and can therefore
      // only get created when the device is created
      internal::init_debug_layer();
#endif

      // 3) we create the command objects.
      // A gpu works with commands, that get added to a command list.
      // command lists themselves are not executable, but need to be added
      // to a command queue, which in turn executes the commands.
      if (!internal::init_command_objects())
      {
        REX_ERROR(LogRhi, "Failed to create command objects");
        return false;
      }

      // 4) Create the global fence
      // The global fence is used for syncronysation between the cpu and the gpu
      if (!internal::init_global_fence())
      {
        REX_ERROR(LogRhi, "Failed to create global fence");
        return false;
      }

      // 5) we need to create a swapchain which is responsible of presenting.
      // There's no benefit in creating all the above systems if we don't have anything
      // to actually present something on screen, that's what the swapchain is for.
      if (!internal::init_swapchain(userData))
      {
        REX_ERROR(LogRhi, "Failed to create swapchain");
        return false;
      }
      
      // 6) We need to create initial heaps that store the data that'll be used for rendering
      // These heaps only hold the data, but letting the graphics pipeline know which resource to use
      // is never done with the heaps directly, but instead are only used to store the data
      // To let graphics pipeline know which resources to use, we use descriptors, which are stored
      // in their own heap
      if (!internal::init_resource_heaps())
      {
        REX_ERROR(LogRhi, "Failed to create resource heaps");
        return false;
      }

      // 7) We need a few heaps to store descriptors which point to the actual resources in other heaps
      // A descriptor is just some metadata about the resource, holding all the information
      // the gpu needs to use a resource.
      // A heap on the gpu is just the same as on the cpu except it's more limited to what it can store
      // You don't get random access into it, a heap can only store specific types and you access into the heap by offset
      // Where each offset is the multiplied by the size of the type the heap stores.
      // The heap doesn't store the content of a resource but stores the view to the resource.
      if (!internal::init_descriptor_heaps())
      {
        REX_ERROR(LogRhi, "Failed to create descriptor heaps");
        return false;
      }

      return true;
    }
  }
}