#include "rex_directx/gfx/dx_gpu_engine.h"

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/log.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_directx/utility/dx_util.h"
#include "rex_directx/utility/d3dx12.h"
#include "rex_directx/dxgi/includes.h"
#include "rex_directx/dxgi/factory.h"
#include "rex_directx/dxgi/adapter.h"
#include "rex_directx/dxgi/adapter_manager.h"
#include "rex_directx/system/dx_device.h"
#include "rex_directx/system/dx_feature_level.h"
#include "rex_directx/system/dx_resource_heap.h"
#include "rex_directx/system/dx_descriptor_heap.h"
#include "rex_directx/resources/dx_texture_2d.h"

namespace rex
{
  namespace gfx
  {
    DEFINE_LOG_CATEGORY(LogDxGpuEngine);

    DxGpuEngine::DxGpuEngine(const renderer::OutputWindowUserData& userData, rsl::unique_ptr<rhi::DxDevice> device, rsl::unique_ptr<dxgi::AdapterManager> adapterManager)
      : GpuEngine(userData)
      , m_device(rsl::move(device))
      , m_heap()
      , m_descriptor_heap_pool()
    {
      // Create a scopeguard that automatically marks initialization as failed
      // This is to make it easy to extend initialization where needed with only
      // needing 1 place where we mark initialization as successful.
      rsl::scopeguard mark_init_failed = [this]() { init_failed(); };

#ifdef REX_ENABLE_DX12_DEBUG_LAYER
      // 2.1) Create the debug layer aftet device gets created
      // The debug layer is linked to the device and can therefore
      // only get created when the device is created
      init_debug_layer();
#endif

      mark_init_failed.release();
    }

    void DxGpuEngine::init_debug_layer()
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
        REX_WARN(LogDxGpuEngine, "Unable to get D3D12 Debug Interface");
      }
    }
    void DxGpuEngine::init_resource_heap()
    {
      CD3DX12_HEAP_DESC desc(100_mib, D3D12_HEAP_TYPE_DEFAULT);

      wrl::ComPtr<ID3D12Heap> d3d_heap;
      if (DX_FAILED(m_device->get()->CreateHeap(&desc, IID_PPV_ARGS(&d3d_heap))))
      {
        REX_ERROR(LogDxGpuEngine, "Failed to create global resource heap");
      }

      m_heap = rsl::make_unique<rhi::ResourceHeap>(d3d_heap, m_device->get());
    }

    wrl::ComPtr<ID3D12Resource> DxGpuEngine::allocate_buffer(rsl::memory_size size)
    {
      return m_heap->create_buffer(size);
    }
    wrl::ComPtr<ID3D12Resource> DxGpuEngine::allocate_texture2d(renderer::TextureFormat format, s32 width, s32 height)
    {
      DXGI_FORMAT d3d_format = d3d::to_dx12(format);
      return m_heap->create_texture2d(d3d_format, width, height);
    }

    rhi::DescriptorHandle DxGpuEngine::create_rtv(const wrl::ComPtr<ID3D12Resource>& texture)
    {
      return m_descriptor_heap_pool.at(D3D12_DESCRIPTOR_HEAP_TYPE_RTV).create_rtv(texture.Get());
    }
    rhi::DescriptorHandle DxGpuEngine::create_texture2d_srv(const wrl::ComPtr<ID3D12Resource>& texture)
    {
      return m_descriptor_heap_pool.at(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).create_texture2d_srv(texture.Get());
    }
    rhi::DescriptorHandle DxGpuEngine::create_cbv(const wrl::ComPtr<ID3D12Resource>& resource)
    {
      return m_descriptor_heap_pool.at(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).create_cbv(resource.Get());
    }

  }
}