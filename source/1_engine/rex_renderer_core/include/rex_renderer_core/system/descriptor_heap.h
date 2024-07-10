#pragma once

#include "rex_std/vector.h"

namespace rex
{
  namespace gfx
  {
    class ResourceView;

    // Base interface for a descriptor heap
    class DescriptorHeap
    {
    public:
      virtual ~DescriptorHeap() = default;

      virtual rsl::unique_ptr<ResourceView> copy_descriptors(const rsl::vector<ResourceView*>& descriptors) = 0;
    };
  }
}