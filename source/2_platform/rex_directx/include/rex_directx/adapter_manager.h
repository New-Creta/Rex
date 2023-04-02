#pragma once

#include "rex_renderer_core/gpu_manager.h"

#include <vector>
#include <memory>
#include <functional>

namespace rex
{
    class Gpu;
    class GpuScorer;

    namespace dxgi
    {
        class GraphicalInfrastructure;
        class Factory;

        using AdapterVec = std::vector<std::unique_ptr<Gpu>>;

        class AdapterManager : public rex::GpuManager
        {
        public:
            AdapterManager(const rex::dxgi::Factory* factory, const GpuScorer& scorer);
            ~AdapterManager() final;

            void load(const rex::dxgi::Factory* factory);

            rex::Gpu* selected() const final;
            rex::Gpu* first() const final;
            const AdapterVec& all() const final;

        private:
            rex::Gpu* m_selected_adapter;
            std::vector<std::unique_ptr<rex::Gpu>> m_adapters;
        };
    }
}