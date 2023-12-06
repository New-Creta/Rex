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
    rex::wrl::com_ptr<DXGIFactoryInterface> create_dxgi_factory(s32 flags)
    {
        rex::wrl::com_ptr<DXGIFactoryInterface> dxgi_factory = nullptr;

        const HRESULT hr = CreateDXGIFactory2(flags, IID_PPV_ARGS(dxgi_factory.GetAddressOf()));
        if (hr != S_OK)
            return nullptr;

        return dxgi_factory;
    }

    //-------------------------------------------------------------------------
    template <typename DXGIFactoryInterface>
    rex::wrl::com_ptr<DXGIFactoryInterface> create_dxgi_factory()
    {
        rex::wrl::com_ptr<DXGIFactoryInterface> dxgi_factory = nullptr;

        const HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(dxgi_factory.GetAddressOf()));
        if (hr != S_OK)
            return nullptr;

        return dxgi_factory;
    }

    //-------------------------------------------------------------------------
    rex::wrl::com_ptr<IDXGIFactory> create_dxgi_factory()
    {
        rex::wrl::com_ptr<IDXGIFactory> dxgi_factory = nullptr;

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
        Factory Factory::create(s32 flags)
        {
            if (wrl::com_ptr<IDXGIFactory> factory = create_dxgi_factory<IDXGIFactory7>(flags))
                return Factory(rsl::move(factory), 7);
            if (wrl::com_ptr<IDXGIFactory> factory = create_dxgi_factory<IDXGIFactory6>(flags))
                return Factory(rsl::move(factory), 6);
            if (wrl::com_ptr<IDXGIFactory> factory = create_dxgi_factory<IDXGIFactory5>(flags))
                return Factory(rsl::move(factory), 5);
            if (wrl::com_ptr<IDXGIFactory> factory = create_dxgi_factory<IDXGIFactory4>(flags))
                return Factory(rsl::move(factory), 4);
            if (wrl::com_ptr<IDXGIFactory> factory = create_dxgi_factory<IDXGIFactory3>(flags))
                return Factory(rsl::move(factory), 3);
            if (wrl::com_ptr<IDXGIFactory> factory = create_dxgi_factory<IDXGIFactory2>(flags))
                return Factory(rsl::move(factory), 2);
            if (wrl::com_ptr<IDXGIFactory> factory = create_dxgi_factory<IDXGIFactory1>())
                return Factory(rsl::move(factory), 1);
            if (wrl::com_ptr<IDXGIFactory> factory = create_dxgi_factory())
                return Factory(rsl::move(factory), 0);

            REX_ASSERT_X(false, "Couldn't create dxgi factory!");
            return Factory();
        }

        Factory::Factory()
            : ComObject(nullptr, 0)
        {}
        //-------------------------------------------------------------------------
        Factory::Factory(wrl::com_ptr<IDXGIFactory>&& object, u32 version)
            : ComObject(rsl::move(object), version)
        {}
    } // namespace dxgi
} // namespace rex