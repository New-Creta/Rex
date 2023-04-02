#include "dxgi/core/dxgiadapter.h"

#include "string/stringconversions.h"

namespace
{
    const uint32 ADAPTER_DESCRIPTION_SIZE = 128;

    template <typename DXGIAdapterDesc>
    rex::Gpu::Description convertDescription(const DXGIAdapterDesc& dxgiDesc)
    {
        rex::Gpu::Description desc;

        desc.name = rex::conversions::toMultiByte(dxgiDesc.Description, ADAPTER_DESCRIPTION_SIZE);

        desc.vendor_id = dxgiDesc.VendorId;
        desc.device_id = dxgiDesc.DeviceId;

        desc.dedicated_video_memory = dxgiDesc.DedicatedVideoMemory;
        desc.dedicated_system_memory = dxgiDesc.DedicatedSystemMemory;
        desc.shared_system_memory = dxgiDesc.SharedSystemMemory;

        return desc;
    }

    rex::Gpu::Description getDescription(const rex::dx::ComPtr<IDXGIAdapter>& adapter)
    {
        rex::Gpu::Description desc;

        // MSDN says that you shouldn't use dxgi 1.0 and dxgi 1.1
        // in the same application.
        // so if the version is 1 or higher, we use dxgi 1.1
        Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter_1;
        adapter.As<IDXGIAdapter1>(&adapter_1);
        if (adapter_1)
        {
            DXGI_ADAPTER_DESC1 dxgi_desc;
            adapter_1->GetDesc1(&dxgi_desc);
            desc = convertDescription(dxgi_desc);
        }
        else
        {
			DXGI_ADAPTER_DESC dxgi_desc;
			adapter->GetDesc(&dxgi_desc);
            desc = convertDescription(dxgi_desc);
        }

        return desc;
    }
}

//-------------------------------------------------------------------------
rex::dxgi::Adapter::Adapter(rex::dx::ComPtr<IDXGIAdapter>&& adapter, uint32 version)
    : rex::Gpu(::getDescription(adapter))
    , ComObject(std::move(adapter), version)
{}

//-------------------------------------------------------------------------
rex::dxgi::Adapter::~Adapter() = default;
