#pragma once

#include "rex_std/vector.h"
#include "rex_std/memory.h"

#include "rex_renderer_core/format_type.h"

namespace rex
{
    enum class FormatType;

    class Gpu;
    class GpuManager;

    class Output;

    struct DisplayMode;

    using OutputVec = rsl::vector<rsl::unique_ptr<Output>> ;

    class OutputManager
    {
    public:
        OutputManager(GpuManager* AdapterManager);
        virtual ~OutputManager();

        virtual void load(rex::GpuManager* manager) = 0;

        virtual DisplayMode highest_resolution(rex::FormatType format) const = 0;
        virtual DisplayMode highest_refreshRate(rex::FormatType format) const = 0;

        virtual DisplayMode highest_resolution(rex::FormatType format, const Gpu* gpu) const = 0;
        virtual DisplayMode highest_refreshRate(rex::FormatType format, const Gpu* gpu) const = 0;

        virtual Output* first() const = 0;
        virtual const OutputVec& all() const = 0;

    protected:
        const GpuManager* adapter_manager() const;

    private:
        GpuManager* m_adapter_manager;
    };
}