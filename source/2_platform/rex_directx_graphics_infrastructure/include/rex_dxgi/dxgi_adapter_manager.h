#pragma once

#include "rex_dxgi/objects/adapter.h"
#include "rex_renderer_core/gpu_scorer.h"
#include "rex_std/vector.h"

namespace rex
{
  namespace dxgi
  {
    class Factory;

    using AdapterVec = rsl::vector<rsl::shared_ptr<Adapter>>;

    class AdapterManager
    {
    public:
      AdapterManager(Factory* factory, const renderer::GpuScorerFn& scorer);
      AdapterManager(const AdapterManager&) = delete;
      AdapterManager(AdapterManager&&)      = delete;
      ~AdapterManager() = default;

      AdapterManager& operator=(const AdapterManager&) = delete;
      AdapterManager& operator=(AdapterManager&&)      = delete;

      bool load_adapters(Factory* factory);

      rsl::shared_ptr<Adapter> selected() const;
      rsl::shared_ptr<Adapter> first() const;
      const AdapterVec& all() const;

    private:
      rsl::shared_ptr<Adapter> m_selected_adapter;
      AdapterVec m_adapters;
    };
  } // namespace dxgi
} // namespace rex