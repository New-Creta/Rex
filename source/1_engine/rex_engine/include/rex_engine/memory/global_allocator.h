#pragma once

#include "rex_engine/memory/tracker_allocator.h"
#include "rex_engine/memory/untracker_allocator.h"

namespace rex
{
  using GlobalAllocator = TrackedAllocator;

  GlobalAllocator& global_allocator();
}