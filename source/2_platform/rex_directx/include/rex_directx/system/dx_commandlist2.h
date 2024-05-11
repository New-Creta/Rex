#pragma once

#include "rex_directx/utility/dx_util.h"

#include "rex_directx/system/dx_command_allocator.h"

namespace rex
{
  namespace rhi
  {
    class CommandList
    {
    public:
      CommandList(const wrl::ComPtr<ID3D12GraphicsCommandList>& commandList);

      void start_recording_commands(CommandAllocator* alloc, ID3D12PipelineState* pso = nullptr);
      void stop_recording_commands();

      ID3D12GraphicsCommandList* get();

    private:
      wrl::ComPtr<ID3D12GraphicsCommandList> m_cmd_list;
      CommandAllocator* m_alloc;
    };
  }
}