#include "rex_renderer_core/rhi/rhi.h"

#include "rex_engine/diagnostics/log.h"

namespace rex
{
  DEFINE_LOG_CATEGORY(LogRhi);

  namespace rhi
  {
    void log_info()
    {
      rhi::Info info = rhi::info();
      REX_INFO(LogRhi, "Adaptor: {}", info.adaptor);
      REX_INFO(LogRhi, "Vendor: {}", info.vendor);
      REX_INFO(LogRhi, "API: {}", info.api);
      REX_INFO(LogRhi, "API Version: {}", info.api_version);
      REX_INFO(LogRhi, "Shader Version: {}", info.shader_version);
      REX_INFO(LogRhi, "Driver Version: {}", info.driver_version);
      REX_INFO(LogRhi, "Available Memory: {}", info.available_memory);
    }
  }

}