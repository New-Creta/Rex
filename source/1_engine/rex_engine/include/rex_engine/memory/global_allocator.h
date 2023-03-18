#pragma once

#include "rex_engine/memory/tracked_allocator.h"
#include "rex_engine/memory/untracker_allocator.h"
#include "rex_engine/memory/debug_allocator.h"

namespace rex
{
#ifdef REX_ENABLE_MEM_TRACKING
  using GlobalAllocator = TrackedAllocator<UntrackedAllocator>;
#else
  using GlobalAllocator = UntrackedAllocator;
#endif

  using GlobalDebugAllocator = DebugAllocator;

  GlobalAllocator& global_allocator();
  GlobalDebugAllocator& global_debug_allocator();
} // namespace rex