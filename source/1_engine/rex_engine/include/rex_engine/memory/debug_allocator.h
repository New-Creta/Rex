#pragma once

#include "rex_std/bonus/attributes.h"
#include "rex_std/bonus/types.h"
#include "rex_std/memory.h"

namespace rex
{
  inline namespace v1
  {
    template <typename Allocator>
    class DebugAllocator
    {
    public:
      using size_type = typename Allocator::size_type;
      using pointer   = typename Allocator::pointer;

      DebugAllocator(Allocator& allocator)
          : m_allocator(rsl::addressof(allocator))
      {
      }

      REX_NO_DISCARD pointer allocate(size_type size)
      {
#ifndef REX_RELEASE
        return m_allocator->allocate(size);
#else
        return nullptr;
#endif
      }
      void deallocate(pointer ptr, size_type size)
      {
        m_allocator->deallocate(ptr, size);
      }

      bool operator==(const DebugAllocator& rhs) const
      {
        return *m_allocator == *rhs.m_allocator;
      }
      bool operator!=(const DebugAllocator& rhs) const
      {
        return !(*this == rhs);
      }

    private:
      Allocator* m_allocator;
    };
  } // namespace v1
} // namespace rex