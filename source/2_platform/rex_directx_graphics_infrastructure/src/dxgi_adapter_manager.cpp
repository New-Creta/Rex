#include "rex_dxgi/dxgi_adapter_manager.h"
#include "rex_dxgi/objects/adapter.h" // IWYU pragma: keep
#include "rex_dxgi/objects/factory.h"
#include "rex_dxgi/dxgi_util.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/platform/win/win_com_ptr.h"
#include "rex_renderer_core/gpu_description.h"
#include "rex_std/bonus/types.h"
#include "rex_std/functional.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace
{
  //-------------------------------------------------------------------------
  rsl::function<HRESULT(UINT, rex::wrl::ComPtr<IDXGIAdapter4>*)> get_enumaration_function(rex::dxgi::Factory* factory)
  {
    rex::wrl::ComPtr<IDXGIFactory6> factory_6 = factory->as<IDXGIFactory6>(); // NOLINT(misc-const-correctness)

    REX_ASSERT_X(factory_6, "IDXGIFactory6 does not exist!");

    return [factory = factory_6.Get()](UINT index, rex::wrl::ComPtr<IDXGIAdapter4>* adapter) { return factory->EnumAdapterByGpuPreference(index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS((*adapter).GetAddressOf())); };
  }

  //-------------------------------------------------------------------------
  rsl::vector<rsl::shared_ptr<rex::dxgi::Adapter>> get_adapters(const rsl::function<HRESULT(UINT, rex::wrl::ComPtr<IDXGIAdapter4>*)>& enumarationFnc)
  {
    uint32 i                                = 0;
    rex::wrl::ComPtr<IDXGIAdapter4> adapter = nullptr;

    rsl::vector<rsl::shared_ptr<rex::dxgi::Adapter>> adapters;
    while(enumarationFnc(i, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
      if(adapter)
      {
        adapters.emplace_back(rsl::make_shared<rex::dxgi::Adapter>(rsl::move(adapter)));
      }

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
    AdapterManager::AdapterManager(Factory* factory, const renderer::GpuScorerFn& scorerFn)
        : m_selected_adapter(nullptr)
    {
      load_adapters(factory);

      REX_ASSERT_X(!m_adapters.empty(), "No adapters found");

      // this can be fixed once we have vector views/ranges
      rsl::vector<renderer::GpuDescription> gpus;
      gpus.reserve(m_adapters.size());
      for(const auto& adapter: m_adapters)
      {
        gpus.push_back(adapter->description());
      }
      const count_t selected_adapter_idx = scorerFn(gpus);
      if(selected_adapter_idx != -1)
      {
        m_selected_adapter = m_adapters[selected_adapter_idx];
      }
    }

    //-------------------------------------------------------------------------
    bool AdapterManager::load_adapters(Factory* factory)
    {
      m_adapters = get_adapters(get_enumaration_function(factory));

      return m_adapters.empty() == false; // NOLINT(readability-simplify-boolean-expr)
    }

    //-------------------------------------------------------------------------
    rsl::shared_ptr<Adapter> AdapterManager::selected() const
    {
      REX_ASSERT_X(m_selected_adapter, "No adapter selected. Call \" select(uint32 adapterID) \" first");

      return m_selected_adapter;
    }
    //-------------------------------------------------------------------------
    rsl::shared_ptr<Adapter> AdapterManager::first() const
    {
      REX_ASSERT_X(!m_adapters.empty(), "No adapters found");

      return m_adapters.front();
    }
    //-------------------------------------------------------------------------
    const rsl::vector<rsl::shared_ptr<Adapter>>& AdapterManager::all() const
    {
      REX_ASSERT_X(!m_adapters.empty(), "No adapters found");

      return m_adapters;
    }
  } // namespace dxgi
} // namespace rex