#include "rex_directx/system/directx_commandlist.h"
#include "rex_directx/system/directx_command_queue.h"

#include "rex_directx/diagnostics/directx_call.h"

#include "rex_std/utility.h"

namespace rex
{
  namespace rhi
  {
    CommandList::CommandList(const wrl::ComPtr<ID3D12GraphicsCommandList>& commandList, const wrl::ComPtr<ID3D12CommandAllocator>& allocator)
      : m_command_list(commandList)
      , m_allocator(allocator)
    {}

    void CommandList::reset(ID3D12PipelineState* pso)
    {
      m_command_list->Reset(m_allocator.Get(), pso);
    }

    void CommandList::exec(CommandQueue* cmdQueue)
    {
      m_command_list->Close();
      cmdQueue->execute(m_command_list.Get());
    }

    ScopedCommandList::ScopedCommandList(CommandList* cmdList, CommandQueue* cmdQueue)
      : m_cmd_list(cmdList)
      , m_cmd_queue(cmdQueue)
    {
      m_cmd_list->reset();
    }

    ScopedCommandList::ScopedCommandList(ScopedCommandList&& other)
      : m_cmd_list(rsl::exchange(other.m_cmd_list, nullptr))
      , m_cmd_queue(rsl::exchange(other.m_cmd_queue, nullptr))
    {}

    ScopedCommandList::~ScopedCommandList()
    {
      if (m_cmd_list)
      {
        m_cmd_list->exec(m_cmd_queue);
      }
    }

    ScopedCommandList& ScopedCommandList::operator=(ScopedCommandList&& other)
    {
      m_cmd_list = rsl::exchange(other.m_cmd_list, nullptr);
      m_cmd_queue = rsl::exchange(other.m_cmd_queue, nullptr);
    }
  }
}