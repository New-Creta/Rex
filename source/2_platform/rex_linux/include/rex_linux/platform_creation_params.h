#pragma once

namespace rex
{
  // can't be in a namespace as they're forward declared in rex namespace
  // the linker needs to think this is in rex namespace.

  struct PlatformCreationParams
  {
  };
} // namespace rex