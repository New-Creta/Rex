#include "rex_directx/command_queue.h"
#include "rex_directx/command_list.h"
#include "rex_directx/directx_call.h"
#include "rex_directx/directx_device.h"
#include "rex_directx/resources/resource_state_tracker.h"
#include "rex_directx/log.h"
#include "rex_directx/threading/thread_util.h"

#include "rex_engine/diagnostics/assert.h"

#include "rex_std/bonus/utility/enum_reflection.h"

#include <mutex>

namespace rex
{
  namespace renderer
  {
    namespace internal
    {
      wrl::ComPtr<ID3D12CommandQueue> create_command_queue(wrl::ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type)
      {
        wrl::ComPtr<ID3D12CommandQueue> d3d12_command_queue;

        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type                     = type;
        desc.Priority                 = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        desc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask                 = 0;

        if(DX_FAILED(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&d3d12_command_queue))))
        {
          REX_ERROR(LogDirectX, "Unable to create the ID3D12CommandQueue");
          return nullptr;
        }

        return d3d12_command_queue;
      }

      wrl::ComPtr<ID3D12Fence> create_fence(wrl::ComPtr<ID3D12Device2> device)
      {
        wrl::ComPtr<ID3D12Fence> fence;

        if(DX_FAILED((device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)))))
        {
          REX_ERROR(LogDirectX, "Unable to create ID3D12Fence");
          return nullptr;
        }

        return fence;
      }
    } // namespace internal

    namespace adaptors
    {
      class MakeCommandList : public CommandList
      {
      public:
        MakeCommandList(Device& device, D3D12_COMMAND_LIST_TYPE type)
            : CommandList(device, type)
        {
        }

        ~MakeCommandList() override = default;
      };
    } // namespace adaptors

    CommandQueue::CommandQueue(Device& device, D3D12_COMMAND_LIST_TYPE type)
        : m_device(device)
        , m_command_list_type(type)
        , m_fence_value(0)
        , m_bprocess_in_flight_command_lists(true)
    {
      auto d3d_device = m_device.d3d_device();

      m_d3d_command_queue = internal::create_command_queue(d3d_device, type);
      REX_ASSERT_X(m_d3d_command_queue != nullptr, "Failed to create ID3D12CommandQueue");
      m_d3d_fence = internal::create_fence(d3d_device);
      REX_ASSERT_X(m_d3d_fence != nullptr, "Failed to create ID3D12Fence");

      switch(type)
      {
        case D3D12_COMMAND_LIST_TYPE_COPY: m_d3d_command_queue->SetName(L"Copy Command Queue"); break;
        case D3D12_COMMAND_LIST_TYPE_COMPUTE: m_d3d_command_queue->SetName(L"Compute Command Queue"); break;
        case D3D12_COMMAND_LIST_TYPE_DIRECT: m_d3d_command_queue->SetName(L"Direct Command Queue"); break;
      }

      // Set the thread name for easy debugging.
      char thread_name[256];
      sprintf_s(thread_name, "Proccess In Flight Command Lists ");
      switch(type)
      {
        case D3D12_COMMAND_LIST_TYPE_DIRECT: strcat_s(thread_name, "(Direct)"); break;
        case D3D12_COMMAND_LIST_TYPE_COMPUTE: strcat_s(thread_name, "(Compute)"); break;
        case D3D12_COMMAND_LIST_TYPE_COPY: strcat_s(thread_name, "(Copy)"); break;
        default: break;
      }

      m_process_in_flight_command_lists_thread = rsl::thread([&] { proccess_in_flight_command_lists(); });

      threading::set_thread_name(m_process_in_flight_command_lists_thread, thread_name);
    }

    CommandQueue::~CommandQueue()
    {
      m_bprocess_in_flight_command_lists = false;
      m_process_in_flight_command_lists_thread.join();
    }

    rsl::shared_ptr<CommandList> CommandQueue::command_list()
    {
      rsl::shared_ptr<CommandList> command_list;

      // If there is a command list on the queue.
      if(!m_available_command_lists.empty())
      {
        m_available_command_lists.try_pop(command_list);
      }
      else
      {
        // Otherwise create a new command list.
        command_list = rsl::make_shared<adaptors::MakeCommandList>(m_device, m_command_list_type);
      }

      return command_list;
    }

    wrl::ComPtr<ID3D12CommandQueue> CommandQueue::d3d_command_queue() const
    {
      return m_d3d_command_queue;
    }

    // Execute a command list.
    // Returns the fence value to wait for for this command list.
    uint64_t CommandQueue::execute_command_list(rsl::shared_ptr<CommandList> commandList)
    {
      return execute_command_lists(rsl::vector<rsl::shared_ptr<CommandList>>({commandList}));
    }

    uint64_t CommandQueue::execute_command_lists(const rsl::vector<rsl::shared_ptr<CommandList>>& commandLists)
    {
      ResourceStateTracker::lock();

      // Command lists that need to put back on the command list queue.
      rsl::vector<rsl::shared_ptr<CommandList>> to_be_queued;
      to_be_queued.reserve(commandLists.size() * 2); // 2x since each command list will have a pending command list.

      // Command lists that need to be executed.
      rsl::vector<ID3D12CommandList*> d3d_command_lists;
      d3d_command_lists.reserve(commandLists.size() * 2); // 2x since each command list will have a pending command list.

      for(auto commandList: commandLists)
      {
        auto pending_command_list = command_list();
        bool has_pending_barriers = commandList->close(pending_command_list);
        pending_command_list->close();

        // If there are no pending barriers on the pending command list, there is no reason to
        // execute an empty command list on the command queue.
        if(has_pending_barriers)
        {
          d3d_command_lists.push_back(pending_command_list->graphics_command_list().Get());
        }

        d3d_command_lists.push_back(commandList->graphics_command_list().Get());

        to_be_queued.push_back(pending_command_list);
        to_be_queued.push_back(commandList);
      }

      UINT num_command_lists = static_cast<UINT>(d3d_command_lists.size());
      m_d3d_command_queue->ExecuteCommandLists(num_command_lists, d3d_command_lists.data());
      uint64_t fence_value = signal();

      ResourceStateTracker::unlock();

      // Queue command lists for reuse.
      for(auto command_list_to_queue: to_be_queued)
      {
        m_in_flight_command_lists.push({fence_value, command_list_to_queue});
      }

      return fence_value;
    }

    u64 CommandQueue::signal()
    {
      u64 fence_value = ++m_fence_value;
      m_d3d_command_queue->Signal(m_d3d_fence.Get(), fence_value);
      return fence_value;
    }

    bool CommandQueue::is_fence_complete(u64 fenceValue) const
    {
      return m_d3d_fence->GetCompletedValue() >= fenceValue;
    }

    void CommandQueue::wait_for_fence_value(u64 fenceValue)
    {
      if(!is_fence_complete(fenceValue))
      {
        auto event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
        if(event)
        {
          m_d3d_fence->SetEventOnCompletion(fenceValue, event);
          ::WaitForSingleObject(event, (rsl::numeric_limits<unsigned long>::max)());

          ::CloseHandle(event);
        }
      }
    }

    void CommandQueue::flush()
    {
      rsl::unique_lock<rsl::mutex> lock(m_process_in_flight_command_lists_thread_mutex);
      m_process_in_flight_command_lists_thread_CV.wait(lock, [this] { return m_in_flight_command_lists.empty(); });

      // In case the command queue was signaled directly
      // using the CommandQueue::Signal method then the
      // fence value of the command queue might be higher than the fence
      // value of any of the executed command lists.
      wait_for_fence_value(m_fence_value);
    }

    void CommandQueue::wait(const CommandQueue& other)
    {
      m_d3d_command_queue->Wait(other.m_d3d_fence.Get(), other.m_fence_value);
    }

    void CommandQueue::proccess_in_flight_command_lists()
    {
      std::unique_lock<rsl::mutex> lock(m_process_in_flight_command_lists_thread_mutex, std::defer_lock);

      while(m_bprocess_in_flight_command_lists)
      {
        CommandListEntry list_entry;

        lock.lock();
        while(m_in_flight_command_lists.try_pop(list_entry))
        {
          auto fence_value  = list_entry.fence_value;
          auto command_list = list_entry.command_list;

          wait_for_fence_value(fence_value);

          command_list->reset();

          m_available_command_lists.push(command_list);
        }
        lock.unlock();

        m_process_in_flight_command_lists_thread_CV.notify_one();

        rsl::this_thread::yield();
      }
    }
  } // namespace renderer
} // namespace rex