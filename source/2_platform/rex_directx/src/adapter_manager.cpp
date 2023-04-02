#include "dxgi/core/dxgiadaptermanager.h"
#include "dxgi/core/dxgiadapter.h"
#include "dxgi/core/dxgifactory.h"

#include "core/highestvramgpuscorer.h"

namespace
{
    //-------------------------------------------------------------------------
    std::function<HRESULT(UINT, IDXGIAdapter4**)> getEnumarationFunction6(const rex::dxgi::Factory* factory)
    {
        rex::dx::ComPtr<IDXGIFactory6> factory_6 = factory->getAs<IDXGIFactory6>();

        RX_ASSERT_X(factory_6, "DXGIFactory 6 does not exist!");

        return [factory = factory_6.Get()](UINT index, IDXGIAdapter4** adapter)
        {
            return factory->EnumAdapterByGpuPreference(index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(adapter));
        };
    }
    //-------------------------------------------------------------------------
    std::function<HRESULT(UINT, IDXGIAdapter1**)> getEnumarationFunction1(const rex::dxgi::Factory* factory)
    {
        rex::dx::ComPtr<IDXGIFactory4> factory_4 = factory->getAs<IDXGIFactory4>();

        RX_ASSERT_X(factory_4, "DXGIFactory 4 does not exist!");

        return [factory = factory_4.Get()](UINT index, IDXGIAdapter1** adapter)
        {
            return factory->EnumAdapters1(index, adapter);
        };
    }
    //-------------------------------------------------------------------------
    std::function<HRESULT(UINT, IDXGIAdapter**)> getEnumarationFunction(const rex::dxgi::Factory* factory)
    {
        RX_ASSERT_X(factory, "DXGIFactory does not exist");

        return [factory = factory->get_c_object()](UINT index, IDXGIAdapter** adapter)
        {
            return factory->EnumAdapters(index, adapter);
        };
    }

    //-------------------------------------------------------------------------
    template<typename DXGIAdapterInterface>
    std::vector<std::unique_ptr<rex::Gpu>> getAdapters(const std::function<HRESULT(UINT, DXGIAdapterInterface**)>& enumarationFnc, uint32 version)
    {
        uint32 i = 0;
		DXGIAdapterInterface* adapter = nullptr;
        
        std::vector<std::unique_ptr<rex::Gpu>> adapters;
        while (enumarationFnc(i, &adapter) != DXGI_ERROR_NOT_FOUND)
        {
            if (adapter)
                adapters.push_back(std::make_unique<rex::dxgi::Adapter>(adapter, version));

            ++i;
        }

        return adapters;
    }
}

//-------------------------------------------------------------------------
rex::dxgi::AdapterManager::AdapterManager(const rex::dxgi::Factory* factory, const GpuScorer& scorer)
    : m_selected_adapter(nullptr)
{
    load(factory);
    m_selected_adapter = scorer.getHighestScoringGpu(m_adapters);
}
//-------------------------------------------------------------------------
rex::dxgi::AdapterManager::~AdapterManager() = default;

//-------------------------------------------------------------------------
void rex::dxgi::AdapterManager::load(const rex::dxgi::Factory* factory)
{
    switch (factory->getVersion())
    {
    case 7:
    case 6:
        m_adapters = std::move(getAdapters<IDXGIAdapter4>(getEnumarationFunction6(factory), 4));
        break;
    case 5:
    case 4:
    case 3:
    case 2:
    case 1:
        m_adapters = std::move(getAdapters<IDXGIAdapter1>(getEnumarationFunction1(factory), 1));
        break;
    case 0:
        m_adapters = std::move(getAdapters<IDXGIAdapter>(getEnumarationFunction(factory), 0));
        break;
    }
}

//-------------------------------------------------------------------------
rex::Gpu* rex::dxgi::AdapterManager::getSelected() const
{
    RX_ASSERT_X(m_selected_adapter, "No adapter selected. Call \" select(uint32 adapterID) \" first");

    return m_selected_adapter;
}
//-------------------------------------------------------------------------
rex::Gpu* rex::dxgi::AdapterManager::getFirst() const
{
    RX_ASSERT_X(m_adapters.size() > 0, "No adapters found");

    return m_adapters[0].get();
}
//-------------------------------------------------------------------------
const std::vector<std::unique_ptr<rex::Gpu>>& rex::dxgi::AdapterManager::getAll() const
{
    RX_ASSERT_X(m_adapters.size() > 0, "No adapters found");

    return m_adapters;
}