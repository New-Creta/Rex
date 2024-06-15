#pragma once

namespace rex
{
  namespace gfx
  {
    // A base interface for an allocator used for queueing gpu commands
    class CommandAllocator
    {
    public:
      virtual ~CommandAllocator() = default;

    private:

    };
  }
}