#include "rex_directx/system/dx_commandlist.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_directx/system/dx_command_queue.h"
#include "rex_directx/system/dx_resource.h"
#include "rex_directx/utility/d3dx12.h"
#include "rex_std/utility.h"

namespace rex
{
  namespace rhi
  {
    CommandList::CommandList(const wrl::ComPtr<ID3D12GraphicsCommandList>& commandList, const wrl::ComPtr<ID3D12CommandAllocator>& allocator)
        : m_command_list(commandList)
        , m_allocator(allocator)
    {
      // Start off in a closed state. This is because the first time we
      // refer to the command list we will Reset it, and it needs to be closed
      // before calling Reset.
      m_command_list->Close();
    }

    void CommandList::reset(ID3D12PipelineState* pso)
    {
      m_allocator->Reset();
      m_command_list->Reset(m_allocator.Get(), pso);
    }

    void CommandList::exec(CommandQueue* cmdQueue)
    {
      m_command_list->Close();
      cmdQueue->execute(m_command_list.Get());
    }

    void CommandList::change_resource_state(Resource* resource, D3D12_RESOURCE_STATES to)
    {
      const D3D12_RESOURCE_STATES from = resource->resource_state();
      if(from != to)
      {
        resource->transition(m_command_list.Get(), to);
      }
    }

    ID3D12GraphicsCommandList* CommandList::get()
    {
      return m_command_list.Get();
    }

    ScopedCommandList::ScopedCommandList(rsl::unique_ptr<CommandList>&& cmdList, CommandQueue* commandQueue)
        : m_cmd_list(rsl::move(cmdList))
        , m_cmd_queue(commandQueue)
    {
      m_cmd_list->reset();
    }

    ScopedCommandList::ScopedCommandList(ScopedCommandList&& other)
        : m_cmd_list(rsl::exchange(other.m_cmd_list, nullptr))
        , m_cmd_queue(rsl::exchange(other.m_cmd_queue, nullptr))
    {
    }

    ScopedCommandList::~ScopedCommandList()
    {
      if(m_cmd_list)
      {
        m_cmd_list->exec(m_cmd_queue);
      }
    }

    ScopedCommandList& ScopedCommandList::operator=(ScopedCommandList&& other)
    {
      m_cmd_list  = rsl::exchange(other.m_cmd_list, nullptr);
      m_cmd_queue = rsl::exchange(other.m_cmd_queue, nullptr);

      return *this;
    }
  } // namespace rhi
} // namespace rex