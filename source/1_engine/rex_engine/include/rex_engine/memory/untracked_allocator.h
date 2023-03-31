#pragma once

#include "rex_engine/types.h"
#include "rex_std_extra/memory/memory_size.h"

namespace rex
{
  class UntrackedAllocator
  {
  public:
    using size_type = card64;
    using pointer   = void*;

    REX_NO_DISCARD pointer allocate(rsl::memory_size size);
    REX_NO_DISCARD pointer allocate(size_type size);

    void deallocate(pointer ptr, rsl::memory_size size);
    void deallocate(pointer ptr, size_type size);
  };
} // namespace rex