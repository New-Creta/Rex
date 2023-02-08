#pragma once

#include "rex_engine/types.h"

namespace rex
{
  struct RendererInfo
  {
    const char8* api_version;
    const char8* shader_version;
    const char8* renderer;
    const char8* vendor;
  };

  namespace renderer
  {
    bool initialize(void* userData, u32 maxCommands);
  } // namespace renderer
} // namespace rex