#pragma once

#include "rex_directx/dxgi/adapter.h"
#include "rex_renderer_core/system/gpu_scorer.h"
#include "rex_std/vector.h"

namespace rex
{
  namespace dxgi
  {
    class Factory;

    using AdapterVec = rsl::vector<Adapter>;

    struct ScoringFuncs
    {
      rsl::function<s32(const GpuDescription&)> calculate_score
    };

    class AdapterManager
    {
    public:
      AdapterManager(const AdapterManager&) = delete;
      AdapterManager(AdapterManager&&)      = delete;
      AdapterManager(Factory* factory, const GpuScorerFn& scorer);
      AdapterManager();
      ~AdapterManager() = default;

      AdapterManager& operator=(const AdapterManager&) = delete;
      AdapterManager& operator=(AdapterManager&&)      = delete;

      bool load_adapters(Factory* factory);

      const Adapter* selected() const;
      const Adapter* first() const;
      const AdapterVec& all() const;

    private:
      const Adapter* m_selected_adapter;
      AdapterVec m_adapters;
    };
  } // namespace dxgi
} // namespace rex