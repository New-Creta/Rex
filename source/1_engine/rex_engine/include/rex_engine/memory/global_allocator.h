#pragma once

#include "rex_engine/memory/tracker_allocator.h"
#include "rex_engine/memory/untracker_allocator.h"

namespace rex
{
#ifdef REX_ENABLE_MEM_TRACKING
  using GlobalAllocator = TrackedAllocator<UntrackedAllocator>;
#else
  using GlobalAllocator = UntrackedAllocator;
#endif

  GlobalAllocator& global_allocator();
} // namespace rex