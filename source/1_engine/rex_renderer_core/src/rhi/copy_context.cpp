#include "rex_renderer_core/rhi/copy_context.h"

namespace rex
{
  namespace rhi
  {
    CopyContext::CopyContext(gfx::GraphicsEngine* owningEngine)
      : GraphicsContext(owningEngine, CommandType::Copy)
    {}
  }
}