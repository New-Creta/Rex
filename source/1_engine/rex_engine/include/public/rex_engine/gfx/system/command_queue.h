#pragma once

#include "rex_engine/engine/types.h"

#include "rex_engine/gfx/core/graphics_engine_type.h"
#include "rex_engine/gfx/system/sync_info_pool.h"
#include "rex_engine/gfx/system/fence.h"

#include "rex_std/bonus/utility.h"

// #TODO: Remaining cleanup of development/Pokemon -> main merge. ID: OBJECT WITH DESTRUCTION CALLBACK

namespace rex
{
  namespace gfx
  {
    DEFINE_YES_NO_ENUM(WaitForFinish);

    class GraphicsContext;
    class CommandQueue
    {
    public:
      CommandQueue(GraphicsEngineType type);
      virtual ~CommandQueue() = default;

      // Halt the cpu until the fence value is reached
      virtual void cpu_wait() = 0;
      // Halt the gpu until the fence value is reached
      virtual void gpu_wait(SyncInfo& sync_info) = 0;
      // Submit all queued commands to the gpu and execute for execution
      virtual ObjectWithDestructionCallback<SyncInfo> execute_context(GraphicsContext* ctx, WaitForFinish waitForFinish) = 0;

      // Returns if the last completed fence is equal or lower than the given fence value
      // Meaning that the commands before the given fence value got signaled have executed
      bool is_fence_completed(u64 fenceValue) const;
      // All commands before this fence value have executed
      u64 last_completed_fence() const;
      // Return the command queue's type
      GraphicsEngineType type() const;
      // Flush all gpu commands and halt the current thread until they're executed
      void flush();

    protected:
      // Return the value of the fence, on the gpu
      virtual u64 gpu_fence_value() const = 0;
      // Create a sync info object with that needs to have the specified fence value for the given fence object
      ObjectWithDestructionCallback<SyncInfo> create_sync_info(u64 fenceValue, Fence* fenceObject);

    private:
      GraphicsEngineType m_type;
      gfx::SyncInfoPool m_sync_info_pool;
    };
  }
};