#pragma once

#include "rex_engine/types.h"
#include "rex_std_extra/memory/memory_size.h"

namespace rex
{
  class TrackedAllocator
  {
  public:
    REX_NO_DISCARD void* allocate(rsl::memory_size size);
    REX_NO_DISCARD void* allocate(card64 size);

    void deallocate(void* ptr, rsl::memory_size size);
    void deallocate(void* ptr, card64 size);
  };
} // namespace rex