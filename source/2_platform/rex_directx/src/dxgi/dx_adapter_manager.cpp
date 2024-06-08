#include "rex_directx/dxgi/dx_adapter.h" // IWYU pragma: keep
#include "rex_directx/dxgi/dx_adapter_manager.h"
#include "rex_directx/dxgi/factory.h"
#include "rex_directx/dxgi/includes.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/platform/win/win_com_ptr.h"
#include "rex_renderer_core/system/gpu_description.h"
#include "rex_std/bonus/types.h"
#include "rex_std/functional.h"

#include <Windows.h>
#include <d3d12.h>

namespace
{
  //-------------------------------------------------------------------------
  rsl::function<HRESULT(UINT, rex::wrl::ComPtr<IDXGIAdapter4>*)> get_enumaration_function6(rex::dxgi::Factory* factory)
  {
    rex::wrl::ComPtr<IDXGIFactory6> factory_6 = factory->as<IDXGIFactory6>(); // NOLINT(misc-const-correctness)

    REX_ASSERT_X(factory_6, "DXGIFactory 6 does not exist!");

    return [factory = factory_6.Get()](UINT index, rex::wrl::ComPtr<IDXGIAdapter4>* adapter) { return factory->EnumAdapterByGpuPreference(index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS((*adapter).GetAddressOf())); };
  }
  //-------------------------------------------------------------------------
  rsl::function<HRESULT(UINT, rex::wrl::ComPtr<IDXGIAdapter1>*)> get_enumaration_function1(rex::dxgi::Factory* factory)
  {
    rex::wrl::ComPtr<IDXGIFactory4> factory_4 = factory->as<IDXGIFactory4>(); // NOLINT(misc-const-correctness)

    REX_ASSERT_X(factory_4, "DXGIFactory 4 does not exist!");

    return [factory = factory_4.Get()](UINT index, rex::wrl::ComPtr<IDXGIAdapter1>* adapter) { return factory->EnumAdapters1(index, (*adapter).GetAddressOf()); };
  }
  //-------------------------------------------------------------------------
  rsl::function<HRESULT(UINT, rex::wrl::ComPtr<IDXGIAdapter>*)> get_enumaration_function(rex::dxgi::Factory* factory)
  {
    REX_ASSERT_X(factory, "DXGIFactory does not exist");

    return [factory = factory->c_ptr()](UINT index, rex::wrl::ComPtr<IDXGIAdapter>* adapter) { return factory->EnumAdapters(index, (*adapter).GetAddressOf()); };
  }

  //-------------------------------------------------------------------------
  template <typename DXGIAdapterInterface>
  rsl::vector<rex::dxgi::Adapter> get_adapters(const rsl::function<HRESULT(UINT, rex::wrl::ComPtr<DXGIAdapterInterface>*)>& enumarationFnc, uint32 version)
  {
    uint32 i                                       = 0;
    rex::wrl::ComPtr<DXGIAdapterInterface> adapter = nullptr;

    rsl::vector<rex::dxgi::Adapter> adapters;
    while(enumarationFnc(i, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
      if(adapter)
        adapters.emplace_back(rsl::move(adapter), version);

      ++i;
    }

    return adapters;
  }
} // namespace

namespace rex
{
  namespace dxgi
  {
    //-------------------------------------------------------------------------
    AdapterManager::AdapterManager(Factory* factory, const GpuScorerFn& scorerFn)
        : m_selected_adapter(nullptr)
    {
      load_adapters(factory);

      REX_ASSERT_X(!m_adapters.empty(), "No adapters found");

      // this can be fixed once we have vector views/ranges
      rsl::vector<GpuDescription> gpus;
      gpus.reserve(m_adapters.size());
      for(const Adapter& adapter: m_adapters)
      {
        gpus.push_back(adapter.description());
      }

      auto it = rsl::max_element(m_adapters.cbegin(), m_adapters.cend(), [&scorerFn](const Adapter& lhs, const Adapter& rhs) { return scorerFn(lhs.description()) < scorerFn(rhs.description()); });
      REX_ASSERT_X(it != m_adapters.cend(), "No adapater found with valid score");
      m_selected_adapter = rsl::iterator_to_pointer(it);      
    }

    //-------------------------------------------------------------------------
    bool AdapterManager::load_adapters(Factory* factory)
    {
      switch(factory->version())
      {
        case 7:
        case 6: m_adapters = rsl::move(get_adapters<IDXGIAdapter4>(get_enumaration_function6(factory), 4)); break;
        case 5:
        case 4:
        case 3:
        case 2:
        case 1: m_adapters = rsl::move(get_adapters<IDXGIAdapter1>(get_enumaration_function1(factory), 1)); break;
        case 0: m_adapters = rsl::move(get_adapters<IDXGIAdapter>(get_enumaration_function(factory), 0)); break;
      }

      return m_adapters.empty() == false; // NOLINT(readability-simplify-boolean-expr)
    }

    //-------------------------------------------------------------------------
    const Adapter* AdapterManager::selected() const
    {
      REX_ASSERT_X(m_selected_adapter, "No adapter selected. Call \" select(uint32 adapterID) \" first");

      return m_selected_adapter;
    }
    //-------------------------------------------------------------------------
    const Adapter* AdapterManager::first() const
    {
      REX_ASSERT_X(!m_adapters.empty(), "No adapters found");

      return &m_adapters.front();
    }
    //-------------------------------------------------------------------------
    const rsl::vector<Adapter>& AdapterManager::all() const
    {
      REX_ASSERT_X(!m_adapters.empty(), "No adapters found");

      return m_adapters;
    }
  } // namespace dxgi
} // namespace rex