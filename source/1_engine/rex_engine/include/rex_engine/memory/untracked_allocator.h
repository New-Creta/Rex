#pragma once

#include "rex_std/bonus/attributes.h"
#include "rex_std/bonus/memory/memory_size.h"
#include "rex_std/bonus/types.h"
#include "rex_std/utility.h"

// IWYU pragma: no_include <new>

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

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args)
    {
      new(static_cast<void*>(p)) U(rsl::forward<Args>(args)...);
    }
    template <typename T>
    void destroy(T* ptr)
    {
      ptr->~T();
    }

    bool operator==(const UntrackedAllocator& /*other*/) const
    {
      return true;
    }
    bool operator!=(const UntrackedAllocator& rhs) const
    {
      return !(*this == rhs);
    }
  };
} // namespace rex