#include "rex_directx/dxgi/adapter_manager.h"

#include <combaseapi.h>                     // for IID_PPV_ARGS_Helper, IID_...
#include <dxgi.h>                           // for IDXGIAdapter, IDXGIAdapter1
#include <dxgi1_4.h>                        // for IDXGIFactory4
#include <dxgi1_6.h>                        // for IDXGIFactory6, IDXGIAdapter4
#include <intsafe.h>                        // for HRESULT, UINT
#include <winerror.h>                       // for DXGI_ERROR_NOT_FOUND

#include "rex_directx/dxgi/factory.h"       // for Factory
#include "rex_engine/diagnostics/assert.h"  // for REX_ASSERT_X
#include "rex_std/functional.h"             // for function
#include "rex_directx/wrl/wrl_types.h"      // for com_ptr
#include "rex_std/bonus/types.h"            // for uint32
#include "rex_directx/dxgi/adapter.h"

namespace
{
  //-------------------------------------------------------------------------
  rsl::function<HRESULT(UINT, IDXGIAdapter4**)> get_enumaration_function6(rex::dxgi::Factory* factory)
  {
    rex::wrl::com_ptr<IDXGIFactory6> factory_6 = factory->as<IDXGIFactory6>(); // NOLINT(misc-const-correctness)

    REX_ASSERT_X(factory_6, "DXGIFactory 6 does not exist!");

    return [factory = factory_6.Get()](UINT index, IDXGIAdapter4** adapter) { return factory->EnumAdapterByGpuPreference(index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(adapter)); };
  }
  //-------------------------------------------------------------------------
  rsl::function<HRESULT(UINT, IDXGIAdapter1**)> get_enumaration_function1(rex::dxgi::Factory* factory)
  {
    rex::wrl::com_ptr<IDXGIFactory4> factory_4 = factory->as<IDXGIFactory4>(); // NOLINT(misc-const-correctness)

    REX_ASSERT_X(factory_4, "DXGIFactory 4 does not exist!");

    return [factory = factory_4.Get()](UINT index, IDXGIAdapter1** adapter) { return factory->EnumAdapters1(index, adapter); };
  }
  //-------------------------------------------------------------------------
  rsl::function<HRESULT(UINT, IDXGIAdapter**)> get_enumaration_function(rex::dxgi::Factory* factory)
  {
    REX_ASSERT_X(factory, "DXGIFactory does not exist");

    return [factory = factory->c_ptr()](UINT index, IDXGIAdapter** adapter) { return factory->EnumAdapters(index, adapter); };
  }

  //-------------------------------------------------------------------------
  template <typename DXGIAdapterInterface>
  rsl::vector<rsl::unique_ptr<rex::Gpu>> get_adapters(const rsl::function<HRESULT(UINT, DXGIAdapterInterface**)>& enumarationFnc, uint32 version)
  {
    uint32 i                      = 0;
    DXGIAdapterInterface* adapter = nullptr;

    rsl::vector<rsl::unique_ptr<rex::Gpu>> adapters;
    while(enumarationFnc(i, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
      if(adapter)
        adapters.push_back(rsl::make_unique<rex::dxgi::Adapter>(adapter, version));

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

      m_selected_adapter = scorerFn(m_adapters);
    }
    //-------------------------------------------------------------------------
    AdapterManager::~AdapterManager() = default;

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
    const Gpu* AdapterManager::selected() const
    {
      REX_ASSERT_X(m_selected_adapter, "No adapter selected. Call \" select(uint32 adapterID) \" first");

      return m_selected_adapter;
    }
    //-------------------------------------------------------------------------
    const Gpu* AdapterManager::first() const
    {
      REX_ASSERT_X(!m_adapters.empty(), "No adapters found");

      return m_adapters[0].get();
    }
    //-------------------------------------------------------------------------
    const rsl::vector<rsl::unique_ptr<Gpu>>& AdapterManager::all() const
    {
      REX_ASSERT_X(!m_adapters.empty(), "No adapters found");

      return m_adapters;
    }
  } // namespace dxgi
} // namespace rex