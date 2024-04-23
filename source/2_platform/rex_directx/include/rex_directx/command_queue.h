#pragma once

#include "rex_directx/directx_util.h"
#include "rex_directx/threading/queue.h"
#include "rex_engine/engine/types.h"

#include "rex_std/memory.h"
#include "rex_std/queue.h"
#include "rex_std/vector.h"
#include "rex_std/atomic.h"
#include "rex_std/condition_variable.h"
#include "rex_std/thread.h"
#include "rex_std/mutex.h"

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

namespace rex
{
  namespace renderer
  {
    class CommandList;
    class Device;

    class CommandQueue
    {
    public:
      rsl::shared_ptr<CommandList> command_list();
      wrl::ComPtr<ID3D12CommandQueue> d3d_command_queue() const;

      // Execute a command list.
      // Returns the fence value to wait for for this command list.
      u64 execute_command_list(rsl::shared_ptr<CommandList> commandList);
      u64 execute_command_lists(const rsl::vector<rsl::shared_ptr<CommandList>>& commandLists);

      u64 signal();
      bool is_fence_complete(u64 fenceValue) const;
      void wait_for_fence_value(u64 fenceValue);
      void flush();

      // Wait for another command queue to finish.
      void wait(const CommandQueue& other);

    protected:
      friend struct rsl::default_delete<CommandQueue>;

      // Only the device can create command queues.
      CommandQueue(Device& device, D3D12_COMMAND_LIST_TYPE type);
      virtual ~CommandQueue();

    private:
      // Free any command lists that are finished processing on the command queue.
      void proccess_in_flight_command_lists();

    private:
      // Keep track of command allocators that are in fligh
      struct CommandAllocatorEntry
      {
        u64 fence_value;
        wrl::ComPtr<ID3D12CommandAllocator> command_allocator;
      };

      // Keep track of command allocators that are "in-flight"
      // The first member is the fence value to wait for, the second is the
      // a shared pointer to the "in-flight" command list.
      struct CommandListEntry
      {
        u64 fence_value;
        rsl::shared_ptr<CommandList> command_list;
      };

      Device& m_device;
      D3D12_COMMAND_LIST_TYPE m_command_list_type;
      wrl::ComPtr<ID3D12CommandQueue> m_d3d_command_queue;
      wrl::ComPtr<ID3D12Fence> m_d3d_fence;
      u64 m_fence_value;

      threading::Queue<CommandListEntry> m_in_flight_command_lists;
      threading::Queue<rsl::shared_ptr<CommandList>> m_available_command_lists;

      // A thread to process in-flight command lists.
      rsl::thread m_process_in_flight_command_lists_thread;
      rsl::atomic<bool> m_bprocess_in_flight_command_lists;
      rsl::mutex m_process_in_flight_command_lists_thread_mutex;
      rsl::condition_variable m_process_in_flight_command_lists_thread_CV;
    };
  } // namespace renderer
} // namespace rex