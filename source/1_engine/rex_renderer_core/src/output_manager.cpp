#include "rex_renderer_core/output_manager.h"

namespace rex
{
    //-------------------------------------------------------------------------
    OutputManager::OutputManager(GpuManager* AdapterManager)
        :m_adapter_manager(AdapterManager)
    {}
    //-------------------------------------------------------------------------
    OutputManager::~OutputManager() = default;

    //-------------------------------------------------------------------------
    const GpuManager* OutputManager::adapter_manager() const
    {
        return m_adapter_manager;
    }
}