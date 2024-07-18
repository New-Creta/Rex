#pragma once

#include "rex_std/vector.h"

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

      virtual rsl::unique_ptr<ResourceView> copy_views(const rsl::vector<ResourceView*>& descriptors) = 0;
      virtual void clear() = 0;
    };
  }
}