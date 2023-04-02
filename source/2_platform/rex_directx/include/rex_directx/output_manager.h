#pragma once


#include "core/outputmanager.h"

#include <map>
#include <vector>
#include <memory>
#include <functional>

namespace rex
{
    class Output;

    namespace dxgi
    {
        class Output;
        class AdapterManager;

        class OutputManager : public rex::OutputManager
        {
        public:
            OutputManager(AdapterManager* manager, uint32 factoryVersion);
            ~OutputManager() final;

			rex::DisplayMode getHighestResolution(rex::FormatType format) const final;
			rex::DisplayMode getHighestRefreshRate(rex::FormatType format) const final;

            rex::DisplayMode getHighestResolution(rex::FormatType format, const Gpu* gpu) const final;
            rex::DisplayMode getHighestRefreshRate(rex::FormatType format, const Gpu* gpu) const final;

            rex::Output* getFirst() const final;
            const OutputVec& getAll() const final;

        private:
            void load(rex::GpuManager* manager) final;

            OutputVec m_outputs;
            uint32 m_factory_version;
        };
    }
}