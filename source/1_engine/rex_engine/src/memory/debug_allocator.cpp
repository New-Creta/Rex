#include "rex_engine/memory/debug_allocator.h"

#include "rex_std/bonus/types.h"

#include <cstdlib>

namespace rex
{
  DebugAllocator::pointer DebugAllocator::allocate(size_type size)
  {
#ifndef REX_RELEASE
    return static_cast<pointer>(malloc(size));
#else
    return nullptr;
#endif
  }
  void DebugAllocator::deallocate(pointer ptr, size_type /*size*/)
  {
    free(ptr);
  }

  bool DebugAllocator::operator==(const DebugAllocator& /*other*/) const
  {
    return true;
  }
  bool DebugAllocator::operator!=(const DebugAllocator& /*other*/) const
  {
    return false;
  }
  }