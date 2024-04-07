#pragma once

#include "rex_engine/engine/defines.h"
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

      DebugAllocator()
          : m_allocator(nullptr)
      {
      }

      explicit DebugAllocator(Allocator& allocator)
          : m_allocator(rsl::addressof(allocator))
      {
      }

      REX_NO_DISCARD pointer allocate(size_type size)
      {
#ifndef REX_BUILD_RELEASE
        return m_allocator->allocate(size);
#else
        REX_UNUSED_PARAM(size);
        return nullptr;
#endif
      }
      void deallocate(pointer ptr, size_type size)
      {
        m_allocator->deallocate(ptr, size);
      }

      template <typename U, typename... Args>
      void construct(U* p, Args&&... args)
      {
        m_allocator->construct(p, rsl::forward<Args>(args)...);
      }
      template <typename T>
      void destroy(T* ptr)
      {
        m_allocator->destroy(ptr);
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