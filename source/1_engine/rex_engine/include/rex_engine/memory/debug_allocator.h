#pragma once

#include "rex_std/bonus/attributes.h"
#include "rex_std/bonus/types.h"

namespace rex
{
  inline namespace v1
  {
    class DebugAllocator
    {
    public:
      using size_type = card64;
      using pointer = void*;

      REX_NO_DISCARD pointer allocate(size_type size);
      void deallocate(pointer ptr, size_type size);

      bool operator==(const DebugAllocator& rhs) const;
      bool operator!=(const DebugAllocator& rhs) const;
    };
  }
}