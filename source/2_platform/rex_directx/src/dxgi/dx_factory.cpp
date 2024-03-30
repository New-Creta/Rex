#include "rex_directx/dxgi/factory.h"

#include "rex_directx/dxgi/util.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_std/memory.h"

#include <Windows.h>
#include <d3d12.h>

namespace
{
    //-------------------------------------------------------------------------
    template <typename DXGIFactoryInterface>
    rex::wrl::ComPtr<DXGIFactoryInterface> create_dxgi_factory(s32 flags)
    {
        rex::wrl::ComPtr<DXGIFactoryInterface> dxgi_factory = nullptr;

        const HRESULT hr = CreateDXGIFactory2(flags, IID_PPV_ARGS(dxgi_factory.GetAddressOf()));
        if (hr != S_OK)
            return nullptr;

        return dxgi_factory;
    }

    //-------------------------------------------------------------------------
    template <typename DXGIFactoryInterface>
    rex::wrl::ComPtr<DXGIFactoryInterface> create_dxgi_factory()
    {
        rex::wrl::ComPtr<DXGIFactoryInterface> dxgi_factory = nullptr;

        const HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(dxgi_factory.GetAddressOf()));
        if (hr != S_OK)
            return nullptr;

        return dxgi_factory;
    }

    //-------------------------------------------------------------------------
    rex::wrl::ComPtr<IDXGIFactory> create_dxgi_factory()
    {
        rex::wrl::ComPtr<IDXGIFactory> dxgi_factory;

        const HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(dxgi_factory.GetAddressOf()));
        if (hr != S_OK)
            return nullptr;

        return dxgi_factory;
    }
} // namespace

namespace rex
{
    namespace dxgi
    {
        //-------------------------------------------------------------------------
        rsl::unique_ptr<Factory> Factory::create(s32 flags)
        {
            if (wrl::ComPtr<IDXGIFactory> factory = create_dxgi_factory<IDXGIFactory7>(flags))
                return rsl::make_unique<Factory>(rsl::move(factory), 7);
            if (wrl::ComPtr<IDXGIFactory> factory = create_dxgi_factory<IDXGIFactory6>(flags))
                return rsl::make_unique<Factory>(rsl::move(factory), 6);
            if (wrl::ComPtr<IDXGIFactory> factory = create_dxgi_factory<IDXGIFactory5>(flags))
                return rsl::make_unique<Factory>(rsl::move(factory), 5);
            if (wrl::ComPtr<IDXGIFactory> factory = create_dxgi_factory<IDXGIFactory4>(flags))
                return rsl::make_unique<Factory>(rsl::move(factory), 4);
            if (wrl::ComPtr<IDXGIFactory> factory = create_dxgi_factory<IDXGIFactory3>(flags))
                return rsl::make_unique<Factory>(rsl::move(factory), 3);
            if (wrl::ComPtr<IDXGIFactory> factory = create_dxgi_factory<IDXGIFactory2>(flags))
                return rsl::make_unique<Factory>(rsl::move(factory), 2);
            if (wrl::ComPtr<IDXGIFactory> factory = create_dxgi_factory<IDXGIFactory1>())
                return rsl::make_unique<Factory>(rsl::move(factory), 1);
            if (wrl::ComPtr<IDXGIFactory> factory = create_dxgi_factory())
                return rsl::make_unique<Factory>(rsl::move(factory), 0);

            REX_ASSERT("Couldn't create dxgi factory!");
            return nullptr;
        }

        Factory::Factory()
          : DxgiObject(nullptr, 0)
        {}

        //-------------------------------------------------------------------------
        Factory::Factory(wrl::ComPtr<IDXGIFactory>&& object, u32 version)
            : DxgiObject(rsl::move(object), version)
        {}
    } // namespace dxgi
} // namespace rex