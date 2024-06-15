#pragma once

namespace rex
{
  namespace gfx
  {
    // Base interface for a fence object
    class Fence
    {
    public:
      virtual ~Fence() = default;
    };
  }
}