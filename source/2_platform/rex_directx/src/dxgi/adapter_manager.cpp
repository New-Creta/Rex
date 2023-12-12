#include "rex_directx/dxgi/adapter_manager.h"

#include "rex_directx/dxgi/adapter.h" // IWYU pragma: keep
#include "rex_directx/dxgi/factory.h"
#include "rex_directx/dxgi/util.h"
#include "rex_directx/wrl/wrl_types.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_renderer_core/gpu_description.h"
#include "rex_std/bonus/types.h"
#include "rex_std/functional.h"

#include <Windows.h>
#include <d3d12.h>

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
  rsl::vector<rex::dxgi::Adapter> get_adapters(const rsl::function<HRESULT(UINT, DXGIAdapterInterface**)>& enumarationFnc, uint32 version)
  {
    uint32 i                      = 0;
    DXGIAdapterInterface* adapter = nullptr;

    rsl::vector<rex::dxgi::Adapter> adapters;
    while(enumarationFnc(i, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
      if(adapter)
        adapters.emplace_back(adapter, version);

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
      const count_t selected_adapter_idx = scorerFn(gpus);
      if(selected_adapter_idx != -1)
      {
        m_selected_adapter = &m_adapters[selected_adapter_idx];
      }
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