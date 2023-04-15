#pragma once

#include "rex_renderer_core/gpu_manager.h"

#include "rex_std/vector.h"
#include "rex_std/memory.h"

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
            AdapterManager(const Factory* factory, const GpuScorer& scorer);
            ~AdapterManager() final;

            bool load_adapters(const Factory* factory);
            bool load_software_adapter(const Factory* factory);

            const Gpu* selected() const final;
            const Gpu* first() const final;
            const Gpu* software() const final;
            const AdapterVec& all() const final;

        private:
            const Gpu* m_selected_adapter;
            rsl::vector<rsl::unique_ptr<Gpu>> m_adapters;
            rsl::unique_ptr<Gpu> m_software_adapter;
        };
    }
}