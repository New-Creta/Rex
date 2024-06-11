#pragma once

#include "rex_engine/engine/types.h"

namespace rex
{
  namespace rhi
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
      SyncInfo()
        : m_fence_value()
        , m_fence_object()
      {
      }

      u64 fence_val() const
      {
        return m_fence_value;
      }
      void* fence_object()
      {
        return m_fence_object;
      }

      void reset(u64 fenceValue, void* fenceObject)
      {
        m_fence_value = fenceValue;
        m_fence_object = fenceObject;
      }

    private:
      u64 m_fence_value; // The fence value on the gpu after the owning context has executed
      void* m_fence_object; // the graphics API specific fence object
    };
  }
}