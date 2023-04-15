#include "rex_directx/dxgi/adapter_manager.h"
#include "rex_directx/dxgi/adapter.h"
#include "rex_directx/dxgi/factory.h"
#include "rex_directx/dxgi/util.h"
#include "rex_directx/log.h"

#include "rex_renderer_core/highest_vram_gpu_scorer.h"

#include "rex_std/functional.h"

namespace
{
    //-------------------------------------------------------------------------
    rsl::function<HRESULT(UINT, IDXGIAdapter4**)> get_enumaration_function6(const rex::dxgi::Factory* factory)
    {
        rex::wrl::com_ptr<IDXGIFactory6> factory_6 = factory->as<IDXGIFactory6>();

        REX_ASSERT_X(factory_6, "DXGIFactory 6 does not exist!");

        return [factory = factory_6.Get()](UINT index, IDXGIAdapter4** adapter)
        {
            return factory->EnumAdapterByGpuPreference(index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(adapter));
        };
    }
    //-------------------------------------------------------------------------
    rsl::function<HRESULT(UINT, IDXGIAdapter1**)> get_enumaration_function1(const rex::dxgi::Factory* factory)
    {
        rex::wrl::com_ptr<IDXGIFactory4> factory_4 = factory->as<IDXGIFactory4>();

        REX_ASSERT_X(factory_4, "DXGIFactory 4 does not exist!");

        return [factory = factory_4.Get()](UINT index, IDXGIAdapter1** adapter)
        {
            return factory->EnumAdapters1(index, adapter);
        };
    }
    //-------------------------------------------------------------------------
    rsl::function<HRESULT(UINT, IDXGIAdapter**)> get_enumaration_function(const rex::dxgi::Factory* factory)
    {
        REX_ASSERT_X(factory, "DXGIFactory does not exist");

        return [factory = factory->c_ptr()](UINT index, IDXGIAdapter** adapter)
        {
            return factory->EnumAdapters(index, adapter);
        };
    }
    
    //-------------------------------------------------------------------------
    template<typename DXGIAdapterInterface>
    rsl::vector<rsl::unique_ptr<rex::Gpu>> get_adapters(const rsl::function<HRESULT(UINT, DXGIAdapterInterface**)>& enumarationFnc, uint32 version)
    {
        uint32 i = 0;
        DXGIAdapterInterface* adapter = nullptr;

        rsl::vector<rsl::unique_ptr<rex::Gpu>> adapters;
        while (enumarationFnc(i, &adapter) != DXGI_ERROR_NOT_FOUND)
        {
            if (adapter)
                adapters.push_back(rsl::make_unique<rex::dxgi::Adapter>(adapter, version));

            ++i;
        }

        return adapters;
    }

    //-------------------------------------------------------------------------
    rsl::unique_ptr<rex::Gpu> get_software_adapter(const rex::dxgi::Factory* factory)
    {
        REX_ASSERT_X(factory, "DXGIFactory does not exist");

        if (factory->version() < 4)
        {
            REX_ERROR(LogDirectX, "Cannot create software rasterizer, incompatible driver");
            return nullptr;
        }

        IDXGIAdapter* adapter = nullptr;

        if(SUCCEEDED(factory->as<IDXGIFactory4>()->EnumWarpAdapter(IID_PPV_ARGS(&adapter))))
        {
            return rsl::make_unique<rex::dxgi::Adapter>(adapter, -1);
        }

        return nullptr;
    }
} // namespace

namespace rex
{
    namespace dxgi
    {
        //-------------------------------------------------------------------------
        AdapterManager::AdapterManager(const Factory* factory, const GpuScorer& scorer)
            : m_selected_adapter(nullptr)
        {
            load_adapters(factory);
            m_selected_adapter = scorer.highest_scoring_gpu(m_adapters);
        }
        //-------------------------------------------------------------------------
        AdapterManager::~AdapterManager() = default;

        //-------------------------------------------------------------------------
        bool AdapterManager::load_adapters(const Factory* factory)
        {
            switch (factory->version())
            {
            case 7:
            case 6:
                m_adapters = rsl::move(get_adapters<IDXGIAdapter4>(get_enumaration_function6(factory), 4));
                break;
            case 5:
            case 4:
            case 3:
            case 2:
            case 1:
                m_adapters = rsl::move(get_adapters<IDXGIAdapter1>(get_enumaration_function1(factory), 1));
                break;
            case 0:
                m_adapters = rsl::move(get_adapters<IDXGIAdapter>(get_enumaration_function(factory), 0));
                break;
            }

            return m_adapters.empty() == false;
        }
        //-------------------------------------------------------------------------
        bool AdapterManager::load_software_adapter(const Factory* factory)
        {
            if (m_software_adapter == nullptr)
            {
                m_software_adapter = get_software_adapter(factory);
                return m_software_adapter != nullptr;
            }
            
            REX_WARN(LogDirectX, "Directx software adapter already loaded");
            return true;
        }

        //-------------------------------------------------------------------------
        const Gpu* AdapterManager::selected() const
        {
            REX_ASSERT_X(m_selected_adapter, "No adapter selected. Call \" select(uint32 adapterID) \" first");

            return m_selected_adapter;
        }
        //-------------------------------------------------------------------------
        const Gpu* AdapterManager::first() const
        {
            REX_ASSERT_X(m_adapters.size() > 0, "No adapters found");

            return m_adapters[0].get();
        }
        //-------------------------------------------------------------------------
        const Gpu* AdapterManager::software() const
        {
            REX_ASSERT_X(m_software_adapter != nullptr, "No valid software adapter");

            return m_software_adapter.get();
        }

        //-------------------------------------------------------------------------
        const rsl::vector<rsl::unique_ptr<Gpu>>& AdapterManager::all() const
        {
            REX_ASSERT_X(m_adapters.size() > 0, "No adapters found");

            return m_adapters;
        }
    } // namespace dxgi
} // namespace rex