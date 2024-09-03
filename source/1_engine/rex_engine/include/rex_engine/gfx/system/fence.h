#pragma once

#include "rex_engine/engine/types.h"

namespace rex
{
  namespace gfx
  {
    class CommandQueue;

    // Base interface for a fence object
    class Fence
    {
    public:
      Fence();
      virtual ~Fence() = default;

      u64 cpu_value() const;

      virtual u64 gpu_value() const = 0;
      virtual u64 inc(CommandQueue* cmdQueue) = 0;
      virtual void wait_for_me() = 0;

    protected:
      void inc_cpu_val();

    private:
      u64 m_cpu_value;
    };
  }
}