#include "rex_dxgi/objects/factory.h"
#include "rex_dxgi/dxgi_util.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_std/memory.h"

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
        namespace adaptors
        {
            class MakeFactory : public Factory
            {
            public:
              MakeFactory()
                  : Factory()
              {
              }
              MakeFactory(wrl::ComPtr<IDXGIFactory>&& factory)
                  : Factory(rsl::move(factory))
              {
              }
            };
        }

        //-------------------------------------------------------------------------
        rsl::unique_ptr<Factory> Factory::create(u32 flags)
        {
            if (wrl::ComPtr<IDXGIFactory> factory = create_dxgi_factory<IDXGIFactory7>(flags))
                return rsl::make_unique<adaptors::MakeFactory>(rsl::move(factory));
            if (wrl::ComPtr<IDXGIFactory> factory = create_dxgi_factory<IDXGIFactory6>(flags))
                return rsl::make_unique<adaptors::MakeFactory>(rsl::move(factory));
            if (wrl::ComPtr<IDXGIFactory> factory = create_dxgi_factory<IDXGIFactory5>(flags))
                return rsl::make_unique<adaptors::MakeFactory>(rsl::move(factory));
            if (wrl::ComPtr<IDXGIFactory> factory = create_dxgi_factory<IDXGIFactory4>(flags))
                return rsl::make_unique<adaptors::MakeFactory>(rsl::move(factory));
            if (wrl::ComPtr<IDXGIFactory> factory = create_dxgi_factory<IDXGIFactory3>(flags))
                return rsl::make_unique<adaptors::MakeFactory>(rsl::move(factory));
            if (wrl::ComPtr<IDXGIFactory> factory = create_dxgi_factory<IDXGIFactory2>(flags))
                return rsl::make_unique<adaptors::MakeFactory>(rsl::move(factory));
            if (wrl::ComPtr<IDXGIFactory> factory = create_dxgi_factory<IDXGIFactory1>())
                return rsl::make_unique<adaptors::MakeFactory>(rsl::move(factory));
            if (wrl::ComPtr<IDXGIFactory> factory = create_dxgi_factory())
                return rsl::make_unique<adaptors::MakeFactory>(rsl::move(factory));

            REX_ASSERT("Couldn't create dxgi factory!");
            return nullptr;
        }

        Factory::Factory()
          : ComObject(nullptr)
        {}

        //-------------------------------------------------------------------------
        Factory::Factory(wrl::ComPtr<IDXGIFactory>&& object)
            : ComObject(rsl::move(object))
        {}
    } // namespace dxgi
} // namespace rex