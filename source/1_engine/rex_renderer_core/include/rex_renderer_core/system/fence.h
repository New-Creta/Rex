#pragma once

namespace rex
{
  namespace rhi
  {
    // Base interface for a fence object
    class Fence
    {
    public:
      virtual ~Fence() = default;
    };
  }
}