#pragma once

#include "rex_renderer_core/gpu_manager.h"
#include "rex_std/memory.h"
#include "rex_std/vector.h"

namespace rex
{
  class Gpu;
  class GpuScorer;

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
      AdapterManager(const Factory* factory, const GpuScorer& scorer);
      ~AdapterManager() override;

      AdapterManager& operator=(const AdapterManager&) = delete;
      AdapterManager& operator=(AdapterManager&&)      = delete;

      bool load_adapters(const Factory* factory);
      bool load_software_adapter(const Factory* factory);

      const Gpu* selected() const override;
      const Gpu* first() const override;
      const Gpu* software() const override;
      const AdapterVec& all() const override;

    private:
      const Gpu* m_selected_adapter;
      rsl::vector<rsl::unique_ptr<Gpu>> m_adapters;
      rsl::unique_ptr<Gpu> m_software_adapter;
    };
  } // namespace dxgi
} // namespace rex