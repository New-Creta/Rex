#pragma once

#include "rex_renderer_core/gpu_manager.h"
#include "rex_renderer_core/gpu_scorer.h"
#include "rex_std/memory.h"
#include "rex_std/vector.h"

namespace rex
{
  class Gpu;

  namespace dxgi
  {
    class GraphicalInfrastructure;
    class Factory;

    using AdapterVec = rsl::vector<rsl::unique_ptr<Gpu>>;

    class AdapterManager : public GpuManager
    {
    public:
      AdapterManager(const AdapterManager&) = delete;
      AdapterManager(AdapterManager&&)      = delete;
      AdapterManager(Factory* factory, const GpuScorerFn& scorer);
      ~AdapterManager() override;

      AdapterManager& operator=(const AdapterManager&) = delete;
      AdapterManager& operator=(AdapterManager&&)      = delete;

      bool load_adapters(Factory* factory);

      const Gpu* selected() const override;
      const Gpu* first() const override;
      const AdapterVec& all() const override;

    private:
      const Gpu* m_selected_adapter;
      rsl::vector<rsl::unique_ptr<Gpu>> m_adapters;
    };
  } // namespace dxgi
} // namespace rex