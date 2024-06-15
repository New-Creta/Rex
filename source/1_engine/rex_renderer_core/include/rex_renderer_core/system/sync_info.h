#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/system/fence.h"

namespace rex
{
  namespace gfx
  {
    // Structure used to syncronise multiple engine with each other
    // eg: If the compute engine needs to wait for the copy engine to finish
    // you somehow need to signal this to the compute engine
    // the sync info is the structure you need to do exactly that.
    // A sync info object is provided with every context.
    // It holds all the information another context (of the same or different context)
    // needs to wait for the first mentioned context to have executed on the gpu.
    class SyncInfo
    {
    public:
      SyncInfo();

      // Return the fence value the fence needs to have on the gpu
      // In order to have the commands that were queued before the fence was set
      // to be executed
      u64 fence_val() const;
      // Return the fence object itself that needs to be checked for the fence value
      Fence* fence_object();

      // Reset the object with a new fence value and fence object
      void reset(u64 fenceValue, Fence* fenceObject);

    private:
      u64 m_fence_value; // The fence value on the gpu after the owning context has executed
      Fence* m_fence_object; // the graphics API specific fence object
    };
  }
}