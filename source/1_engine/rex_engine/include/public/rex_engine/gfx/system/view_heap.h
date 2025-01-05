#pragma once

#include "rex_std/vector.h"
#include "rex_std/memory.h"

namespace rex
{
  namespace gfx
  {
    class ResourceView;

    // Base interface for a descriptor heap
    class ViewHeap
    {
    public:
      virtual ~ViewHeap() = default;

      // Copy the given views into this heap
      virtual rsl::unique_ptr<ResourceView> copy_views(const rsl::vector<const ResourceView*>& views) = 0;

      // Reset the descriptor heap
      // This will cause new descriptor to be allocated from the beginning of the heap
      virtual void clear() = 0;
    };
  }
}