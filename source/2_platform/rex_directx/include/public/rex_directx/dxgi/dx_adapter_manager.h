#pragma once

#include "rex_directx/dxgi/dx_adapter.h"
#include "rex_engine/gfx/system/gpu_scorer.h"
#include "rex_std/vector.h"

namespace rex
{
  namespace gfx
  {
    namespace dxgi
    {
      class Factory;

      class AdapterManager
      {
      public:
        AdapterManager(const AdapterManager&) = delete;
        AdapterManager(AdapterManager&&) = delete;
        AdapterManager(Factory* factory, const GpuScorerFn& scorer);
        ~AdapterManager() = default;

        AdapterManager& operator=(const AdapterManager&) = delete;
        AdapterManager& operator=(AdapterManager&&) = delete;

        bool load_adapters(Factory* factory);

        const Adapter* selected() const;
        const Adapter* first() const;
        const rsl::vector<Adapter>& all() const;

      private:
        const Adapter* m_selected_adapter;
        rsl::vector<Adapter> m_adapters;
      };
    }
  } // namespace dxgi
} // namespace rex