#include "rex_directx/system/dx_commandlist2.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  namespace rhi
  {
    CommandList2::CommandList2(const wrl::ComPtr<ID3D12GraphicsCommandList>& commandList)
      : m_cmd_list(commandList)
      , m_alloc(nullptr)
    {
      m_cmd_list->Close();
    }

    void CommandList2::start_recording_commands(CommandAllocator* alloc, ID3D12PipelineState* pso)
    {
      REX_ASSERT_X(alloc != nullptr, "The allocator for a commandlist cannot be null");
      REX_ASSERT_X(m_alloc == nullptr, "There's already an allocator assigned to this commandlist");

      m_alloc = alloc;

      alloc->get()->Reset();
      m_cmd_list->Reset(m_alloc->get(), pso);
    }
    void CommandList2::stop_recording_commands()
    {
      m_alloc = nullptr;
      m_cmd_list->Close();
    }

    ID3D12GraphicsCommandList* CommandList2::get()
    {
      return m_cmd_list.Get();
    }

  }
}