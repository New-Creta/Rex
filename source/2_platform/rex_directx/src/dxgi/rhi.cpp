#include "rex_directx/dxgi/rhi.h"

#include "rex_directx/dxgi/factory.h"
#include "rex_directx/dxgi/adapter_manager.h"
#include "rex_directx/dxgi/adapter.h"

#include "rex_directx/utility/directx_util.h"

#include "rex_directx/diagnostics/directx_call.h"
#include "rex_directx/diagnostics/log.h"
#include "rex_directx/system/directx_device.h"
#include "rex_directx/system/directx_feature_level.h"
#include "rex_directx/system/directx_command_queue.h"
#include "rex_directx/system/directx_commandlist.h"
#include "rex_directx/system/directx_descriptor_heap.h"
#include "rex_directx/system/directx_resource_heap.h"
#include "rex_directx/system/directx_resource.h"
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
      rsl::unique_ptr<RenderHardwareInfrastructure> g_rhi;
    }

    class RenderHardwareInfrastructure* init(const renderer::OutputWindowUserData& userData)
    {
      internal::g_rhi = rsl::make_unique<RenderHardwareInfrastructure>(userData);

      if (!internal::g_rhi->init_successful())
      {
        shutdown();
      }

      return internal::g_rhi.get();
    }

    void shutdown()
    {
      internal::g_rhi.reset();
    }

    RenderHardwareInfrastructure::RenderHardwareInfrastructure(const renderer::OutputWindowUserData& userData)
      : m_is_initialized(false)
    {
      // Create a scopeguard that automatically marks initialization as failed
      // This is to make it easy to extend initialization where needed with only
      // needing 1 place where we mark initialization as successful.
      rsl::scopeguard mark_init_failed = [this]() { m_is_initialized = false; };

      // 1) we need to init the dxgi factory.
      // This is the system we use to create most other systems.
      if (!init_dxgi_factory())
      {
        REX_ERROR(LogRhi, "Failed to create DXGI Factory");
        return;
      }

#ifdef REX_ENABLE_DX12_DEBUG_LAYER
      // 1.1) Create the debug controller before the device gets created
      // This way we can have some additional debugging information if something goes wrong
      init_debug_controller();
#endif

      // 2) we need to init the device.
      // A DirectX Device is used for creation of other resources
      // You can think of it as an abstraction of the GPU,
      // but just an abstraction over resource creation,
      // not an abstraction of the gpu itself, that's what an IDXGIAdapter is for.
      if (!init_d3d_device())
      {
        REX_ERROR(LogRhi, "Failed to create D3D Device");
        return;
      }

#ifdef REX_ENABLE_DX12_DEBUG_LAYER
      // 2.1) Create the debug layer aftet device gets created
      // The debug layer is linked to the device and can therefore
      // only get created when the device is created
      init_debug_layer();
#endif

      // 3) we create the command queue.
      // The command queue is used to send commands to the gpu
      // It also holds the fence for syncronysation between cpu and gpu
      if (!init_command_queue())
      {
        REX_ERROR(LogRhi, "Failed to create command queue");
        return;
      }

      // 4) Create the global command list
      // A gpu works with commands, that get added to a command list.
      // command lists themselves are not executable, but need to be added
      // to a command queue, which in turn executes the commands.
      if (!init_command_list())
      {
        REX_ERROR(LogRhi, "Failed to create command list");
        return;
      }

      // 5) we need to create a swapchain which is responsible of presenting.
      // There's no benefit in creating all the above systems if we don't have anything
      // to actually present something on screen, that's what the swapchain is for.
      if (!init_swapchain(userData))
      {
        REX_ERROR(LogRhi, "Failed to create swapchain");
        return;
      }

      // 6) We need to create initial heaps that store the data that'll be used for rendering
      // These heaps only hold the data, but letting the graphics pipeline know which resource to use
      // is never done with the heaps directly, but instead are only used to store the data
      // To let graphics pipeline know which resources to use, we use descriptors, which are stored
      // in their own heap
      if (!init_resource_heaps())
      {
        REX_ERROR(LogRhi, "Failed to create resource heaps");
        return;
      }

      // 7) We need a few heaps to store descriptors which point to the actual resources in other heaps
      // A descriptor is just some metadata about the resource, holding all the information
      // the gpu needs to use a resource.
      // A heap on the gpu is just the same as on the cpu except it's more limited to what it can store
      // You don't get random access into it, a heap can only store specific types and you access into the heap by offset
      // Where each offset is the multiplied by the size of the type the heap stores.
      // The heap doesn't store the content of a resource but stores the view to the resource.
      if (!init_descriptor_heaps())
      {
        REX_ERROR(LogRhi, "Failed to create descriptor heaps");
        return;
      }

      // 8) Now create the views for the swapchain's back buffers
      // These views are needed for basic presenting as the GPU
      // doesn't interact with resources directly but uses views instead
      if (!init_swapchain_buffer_views(userData))
      {
        REX_ERROR(LogRhi, "Failed to create swapchain buffer views");
        return;
      }

      // release scopeguard so that init gets marked successful
      mark_init_failed.release();
    }
    RenderHardwareInfrastructure::~RenderHardwareInfrastructure()
    {
      if (m_device)
      {
        // A command queue needs to be flushed to shutdown properly
        m_command_queue->flush();

        // Now that all gpu commands are cleared, we can release all resources
      }
    }

    bool RenderHardwareInfrastructure::init_successful() const
    {
      return m_is_initialized;
    }

    void RenderHardwareInfrastructure::reset_command_list()
    {
      m_command_list->reset();
    }

    void RenderHardwareInfrastructure::exec_command_list()
    {
      m_command_list->close();
      m_command_list->exec(m_command_queue.get());
    }
    void RenderHardwareInfrastructure::flush_command_queue()
    {
      m_command_queue->flush();
    }

    // DXGI Factory
    s32 RenderHardwareInfrastructure::init_debug_interface()
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

      if (DX_SUCCESS(DXGIGetDebugInterface1(0, IID_PPV_ARGS(m_debug_info_queue.GetAddressOf()))))
      {
        dxgi_factory_flags = DXGI_CREATE_FACTORY_DEBUG;

        m_debug_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_MESSAGE, globals::g_enable_dxgi_severity_message);
        m_debug_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_INFO, globals::g_enable_dxgi_severity_info);
        m_debug_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING, globals::g_enable_dxgi_severity_warning);
        m_debug_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, globals::g_enable_dxgi_severity_error);
        m_debug_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, globals::g_enable_dxgi_severity_corruption);

        rsl::array<DXGI_INFO_QUEUE_MESSAGE_ID, 1> dxgi_hide = {
            80 /* IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides. */,
        };
        DXGI_INFO_QUEUE_FILTER dxgi_filter = {};
        dxgi_filter.DenyList.NumIDs = rsl::safe_numeric_cast<u32>(dxgi_hide.size());
        dxgi_filter.DenyList.pIDList = dxgi_hide.data();

        m_debug_info_queue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &dxgi_filter);
      }
      else
      {
        REX_WARN(LogRhi, "Unable to get GXGI Debug Interface");
      }

      return dxgi_factory_flags;
    }
    bool RenderHardwareInfrastructure::init_dxgi_factory()
    {
      s32 dxgi_factory_flags = 0;

#ifdef REX_ENABLE_DXGI_DEBUG_LAYER
      dxgi_factory_flags = init_debug_interface();
#endif

      m_factory = dxgi::Factory::create(dxgi_factory_flags);
      return true;
    }
    void RenderHardwareInfrastructure::init_debug_controller()
    {
      // Enable extra debugging and send debug messages to the VC++ output window
      rex::wrl::ComPtr<ID3D12Debug> debug_controller;
      if (DX_SUCCESS(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller))))
      {
        debug_controller->EnableDebugLayer();
      }
      else
      {
        REX_WARN(LogRhi, "Failed to create DX debug controller");
      }
    }

    // D3D Device
    count_t RenderHardwareInfrastructure::highest_scoring_gpu(const rsl::vector<GpuDescription>& gpus)
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
    void RenderHardwareInfrastructure::init_debug_layer()
    {
      // Device needs to exist before we can query this
      rex::wrl::ComPtr<ID3D12InfoQueue> dx12_info_queue;
      if (DX_SUCCESS(m_device->get()->QueryInterface(IID_PPV_ARGS(dx12_info_queue.GetAddressOf()))))
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
        REX_WARN(LogRhi, "Unable to get D3D12 Debug Interface");
      }
    }

    bool RenderHardwareInfrastructure::init_d3d_device()
    {
      // Find highest scoring gpu
      const dxgi::AdapterManager adapter_manager(m_factory.get(), [this](const rsl::vector<GpuDescription>& gpus) { return highest_scoring_gpu(gpus); });
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

      m_device = rsl::make_unique<renderer::DirectXDevice>(device, feature_level, selected_gpu);
      return true;
    }

    // D3D Command Objects
    bool RenderHardwareInfrastructure::init_command_queue()
    {
      // Command Queue
      wrl::ComPtr<ID3D12CommandQueue> command_queue;
      D3D12_COMMAND_QUEUE_DESC queue_desc = {};
      queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
      queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
      if (DX_FAILED(m_device->get()->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(command_queue.GetAddressOf()))))
      {
        REX_ERROR(LogRhi, "Failed to create command queue");
        return false;
      }

      rhi::set_debug_name_for(command_queue.Get(), "Global Command Queue");

      // Fence
      wrl::ComPtr<ID3D12Fence> fence;
      if (DX_FAILED(m_device->get()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))))
      {
        REX_ERROR(LogRhi, "Failed to create DX fence, to synchronize CPU/GPU");
        return false;
      }

      rhi::set_debug_name_for(fence.Get(), "Global Fence");
      m_command_queue = rsl::make_unique<CommandQueue>(command_queue, fence);

      return true;
    }

    bool RenderHardwareInfrastructure::init_command_list()
    {
      // Command Allocator
      wrl::ComPtr<ID3D12CommandAllocator> allocator;
      if (DX_FAILED(m_device->get()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(allocator.GetAddressOf()))))
      {
        REX_ERROR(LogRhi, "Failed to create command allocator");
        return false;
      }

      rhi::set_debug_name_for(allocator.Get(), "Global Command Allocator");

      // Command List
      wrl::ComPtr<ID3D12CommandList> cmd_list;
      if (DX_FAILED(m_device->get()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator.Get(), nullptr, IID_PPV_ARGS(cmd_list.GetAddressOf()))))
      {
        REX_ERROR(LogRhi, "Failed to create command list");
        return false;
      }

      rhi::set_debug_name_for(cmd_list.Get(), "Global Command List");
      m_command_list = rsl::make_unique<CommandList>(cmd_list, allocator);

      // Start off in a closed state. This is because the first time we
      // refer to the command list we will Reset it, and it needs to be closed
      // before calling Reset.
      if (!m_command_list->close())
      {
        REX_ERROR(LogRhi, "Failed to close command list");
        return false;
      }

      return true;
    }

    // DXGI Swapchain
    bool RenderHardwareInfrastructure::init_swapchain(const renderer::OutputWindowUserData& userData)
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
      rex::wrl::ComPtr<IDXGIFactory> dxgi_factory = m_factory->as<IDXGIFactory>();
      rex::wrl::ComPtr<IDXGISwapChain> swapchain;
      if (DX_FAILED(dxgi_factory->CreateSwapChain(m_command_queue->get(), &sd, swapchain.GetAddressOf())))
      {
        REX_ERROR(LogRhi, "Failed to create swap chain");
        return false;
      }

      rhi::set_debug_name_for(swapchain.Get(), "SwapChain");

      m_swapchain = rsl::make_unique<renderer::Swapchain>(swapchain);
      return true;
    }

    // Resource Heaps
    bool RenderHardwareInfrastructure::init_resource_heaps()
    {
      // Nothing to implement at the moment
      CD3DX12_HEAP_DESC desc(1_kib, D3D12_HEAP_TYPE_DEFAULT);

      wrl::ComPtr<ID3D12Heap> heap;
      if (DX_FAILED(m_device->get()->CreateHeap(&desc, IID_PPV_ARGS(&heap))))
      {
        REX_ERROR(LogRhi, "Failed to create global resource heap");
        return false;
      }

      m_heap = rsl::make_unique<ResourceHeap>(heap, m_device);
      return true;
    }

    // Descriptor Heaps
    bool RenderHardwareInfrastructure::init_descriptor_heap(D3D12_DESCRIPTOR_HEAP_TYPE type, s32 numDescriptors)
    {
      D3D12_DESCRIPTOR_HEAP_DESC desc{};

      desc.Type = type;
      desc.NumDescriptors = numDescriptors;
      desc.Flags = type != D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
        ? D3D12_DESCRIPTOR_HEAP_FLAG_NONE
        : D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
      desc.NodeMask = 0; // For single-adapter operation, set this to zero. ( https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_descriptor_heap_desc )

      wrl::ComPtr<ID3D12DescriptorHeap> desc_heap;
      if (DX_FAILED(m_device->get()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&desc_heap))))
      {
        REX_ERROR(LogRhi, "Failed to create descriptor heap for type: {}", rsl::enum_refl::enum_name(desc.Type));
        return false;
      }

      rsl::string_view type_str = rsl::enum_refl::enum_name(type);
      rhi::set_debug_name_for(desc_heap.Get(), rsl::format("Descriptor Heap Element - {}", type_str));
      s32 desc_size = m_device->get()->GetDescriptorHandleIncrementSize(type);
      s32 total_size = desc_size * numDescriptors;

      REX_LOG(LogRhi, "Created {0} ( num: {1} descriptors, desc size: {2} bytes, total size: {3} bytes) ", type_str, numDescriptors, desc_size, total_size);

      return true;
    }
    bool RenderHardwareInfrastructure::init_descriptor_heaps()
    {
      if (!init_descriptor_heap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, s_num_rtv_descs))
      {
        REX_ERROR(LogRhi, "Failed to create descriptor heap for RTV");
        return false;
      }
      if (!init_descriptor_heap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, s_num_dsv_descs))
      {
        REX_ERROR(LogRhi, "Failed to create descriptor heap for RTV");
        return false;
      }
      if (!init_descriptor_heap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, s_num_cbv_descs))
      {
        REX_ERROR(LogRhi, "Failed to create descriptor heap for RTV");
        return false;
      }

      return true;
    }

    // Swapchain buffer views
    bool RenderHardwareInfrastructure::init_swapchain_buffer_views(const renderer::OutputWindowUserData& userData)
    {
      if (!init_swapchain_rtvs(userData))
      {
        REX_ERROR(LogRhi, "Failed to create swapchain render target views");
        return false;
      }

      if (!init_swapchain_dsvs(userData))
      {
        REX_ERROR(LogRhi, "Failed to create swapchain depth stencil views");
        return false;
      }

      return true;
    }
    bool RenderHardwareInfrastructure::init_swapchain_rtvs(const renderer::OutputWindowUserData& userData)
    {
      s32 width = userData.window_width;
      s32 height = userData.window_height;
      if (DX_FAILED(m_swapchain->resize_buffers(width, height, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH)))
      {
        REX_ERROR(LogRhi, "Failed to resize swapchain buffers");
        return false;
      }

      for (s32 i = 0; i < m_swapchain->buffer_count(); ++i)
      {
        wrl::ComPtr<ID3D12Resource> buffer = m_swapchain->get_buffer(i);
        set_debug_name_for(buffer.Get(), rsl::format("Swapchain Back Buffer {}", i));
        m_descriptor_heap_pool[D3D12_DESCRIPTOR_HEAP_TYPE_RTV].create_rtv(Resource(buffer));
      }

      return true;
    }
    bool RenderHardwareInfrastructure::init_swapchain_dsvs(const renderer::OutputWindowUserData& userData)
    {
      s32 width = userData.window_width;
      s32 height = userData.window_height;

      Resource depth_stencil_buffer = m_heap->create_depth_stencil_resource(width, height);
      set_debug_name_for(depth_stencil_buffer.get(), "Swapchain Depth Stencil Buffer");
      m_descriptor_heap_pool[D3D12_DESCRIPTOR_HEAP_TYPE_DSV].create_dsv(depth_stencil_buffer);
      
      CD3DX12_RESOURCE_BARRIER depth_write_transition = CD3DX12_RESOURCE_BARRIER::Transition(depth_stencil_buffer.get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);      
      m_command_list->get()->ResourceBarrier(1, &depth_write_transition);

      return true;
    }

    ScopedCommandList RenderHardwareInfrastructure::create_scoped_cmd_list()
    {
      return ScopedCommandList(m_command_list.get(), m_command_queue.get());
    }

  }
}