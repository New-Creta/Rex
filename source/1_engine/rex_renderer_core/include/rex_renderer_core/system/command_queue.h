#pragma once

#include "rex_engine/engine/types.h"

#include "rex_renderer_core/rhi/command_type.h"
#include "rex_renderer_core/gfx/sync_info_pool.h"

namespace rex
{
  namespace rhi
  {
    class CommandQueue
    {
    public:
      CommandQueue(CommandType type);
      virtual ~CommandQueue() = default;

      // Halt the cpu until the fence value is reached
      virtual void cpu_wait(u64 fenceValue) = 0;
      // Halt the gpu until the fence value is reached
      virtual void gpu_wait(SyncInfo& sync_info) = 0;
      // Returns true if the fence value has been reached already on the gpu
      virtual bool is_fence_completed(u64 fenceValue) const = 0;
      // Returns the fence value of the last completed set of commands

      virtual ScopedPoolObject<SyncInfo> execute_context(GraphicsContext* ctx) = 0;

      u64 last_completed_fence() const;
      CommandType type() const;
      // Returns the next fence value. This is the fence value that'll be signaled
      // the next time an commandlist is executed
      u64 next_fence_value() const;

    protected:
      // Increases the next fence value and returns the old value
      u64 inc_fence();

      virtual u64 gpu_fence_value() const = 0;

      template <typename SyncInfoClass>
      ScopedPoolObject<SyncInfo> create_sync_info(u64 fenceValue, void* fenceObject)
      {
        return m_sync_info_pool.request_sync_info<SyncInfoClass>(fenceValue, fenceObject);
      }

    private:
      u64 m_next_fence_value; // this fence value gets increment each time the command queue executes a commandlist
      CommandType m_type;
      gfx::SyncInfoPool m_sync_info_pool;
    };
  }
};