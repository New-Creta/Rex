#pragma once

#include "rex_directx/utility/dx_util.h"

namespace rex
{
  namespace rhi
  {
    class CommandQueue;
    class Resource;

    class CommandList
    {
    public:
      CommandList(const wrl::ComPtr<ID3D12GraphicsCommandList>& commandList, const wrl::ComPtr<ID3D12CommandAllocator>& allocator);

      void reset(ID3D12PipelineState* pso = nullptr);
      void exec(CommandQueue* cmdQueue);

      void change_resource_state(Resource* resource, D3D12_RESOURCE_STATES to);

      ID3D12GraphicsCommandList* get();

    private:
      wrl::ComPtr<ID3D12GraphicsCommandList> m_command_list;
      wrl::ComPtr<ID3D12CommandAllocator> m_allocator;
    };

    class ScopedCommandList
    {
    public:
      ScopedCommandList(rsl::unique_ptr<CommandList>&& cmdList, CommandQueue* commandQueue);
      ScopedCommandList(const ScopedCommandList&) = delete;
      ScopedCommandList(ScopedCommandList&& other);
      ~ScopedCommandList();

      ScopedCommandList& operator=(const ScopedCommandList&) = delete;
      ScopedCommandList& operator=(ScopedCommandList&& other);

    private:
      rsl::unique_ptr<CommandList> m_cmd_list;
      CommandQueue* m_cmd_queue;
    };
  } // namespace rhi
} // namespace rex