#include "rex_directx/dxgi/factory.h"
#include "rex_directx/dxgi/util.h"

namespace
{
    //-------------------------------------------------------------------------
    template<typename DXGIFactoryInterface>
    rex::wrl::com_ptr<DXGIFactoryInterface> create_dxgi_factory()
    {
        rex::wrl::com_ptr<DXGIFactoryInterface> dxgi_factory = nullptr;

        HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(dxgi_factory.GetAddressOf()));
        if (hr != S_OK)
            return nullptr;

        return dxgi_factory;
    }

    //-------------------------------------------------------------------------
    template<>
    rex::wrl::com_ptr<IDXGIFactory> create_dxgi_factory()
    {
        rex::wrl::com_ptr<IDXGIFactory> dxgi_factory = nullptr;

        HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(dxgi_factory.GetAddressOf()));
        if (hr != S_OK)
            return nullptr;

        return dxgi_factory;
    }
}

namespace rex
{
    namespace dxgi
    {
        //-------------------------------------------------------------------------
        rsl::unique_ptr<Factory> Factory::create()
        {
            wrl::com_ptr<IDXGIFactory> factory;

            if (factory = create_dxgi_factory<IDXGIFactory7>()) return rsl::make_unique<Factory>(rsl::move(factory), 7);
            if (factory = create_dxgi_factory<IDXGIFactory6>()) return rsl::make_unique<Factory>(rsl::move(factory), 6);
            if (factory = create_dxgi_factory<IDXGIFactory5>()) return rsl::make_unique<Factory>(rsl::move(factory), 5);
            if (factory = create_dxgi_factory<IDXGIFactory4>()) return rsl::make_unique<Factory>(rsl::move(factory), 4);
            if (factory = create_dxgi_factory<IDXGIFactory3>()) return rsl::make_unique<Factory>(rsl::move(factory), 3);
            if (factory = create_dxgi_factory<IDXGIFactory2>()) return rsl::make_unique<Factory>(rsl::move(factory), 2);
            if (factory = create_dxgi_factory<IDXGIFactory1>()) return rsl::make_unique<Factory>(rsl::move(factory), 1);
            if (factory = create_dxgi_factory<IDXGIFactory>())  return rsl::make_unique<Factory>(rsl::move(factory), 0);

            //assert( false, "Couldn't create dxgi factory!");
            return nullptr;
        }

        //-------------------------------------------------------------------------
        Factory::Factory(wrl::com_ptr<IDXGIFactory>&& object, uint32 version)
            : ComObject(rsl::move(object), version)
        {}
        //-------------------------------------------------------------------------
        Factory::~Factory() = default;
    }
}