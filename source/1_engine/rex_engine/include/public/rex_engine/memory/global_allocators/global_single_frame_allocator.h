#pragma once

#include "rex_engine/engine/types.h"

namespace rex
{
  // #TODO: Remaining cleanup of development/Pokemon -> main merge. ID: HEAP AND ALLOCATORS

	class GlobalSingleFrameAllocator
	{
	public:
    void* allocate(const s32 count);    // deallocates the storage reference by the pointer p.
    void deallocate(void* const ptr, s32 count);

    s32 max_size() const;

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args)
    {
      new(static_cast<void*>(p)) U(rsl::forward<Args>(args)...);
    }
    template <typename U>
    void destroy(U* p)
    {
      p->~U();
    }
	};

  constexpr bool operator==(const GlobalSingleFrameAllocator& /*unused*/, const GlobalSingleFrameAllocator& /*unused*/)
  {
    return true;
  }
  constexpr bool operator!=(const GlobalSingleFrameAllocator& /*unused*/, const GlobalSingleFrameAllocator& /*unused*/)
  {
    return false;
  }
}