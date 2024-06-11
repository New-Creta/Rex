#pragma once

#include "rex_renderer_core/gfx/graphics.h"
#include "rex_renderer_core/rhi/copy_context.h"
#include "rex_renderer_core/gfx/copy_engine.h"

#include "rex_directx/utility/dx_util.h"

namespace rex
{
  namespace rhi
  {
    class CommandAllocator;
    class DxCopyEngine;

    class DxCopyContext : public CopyContext
    {
    public:
      DxCopyContext(gfx::GraphicsEngine* owningEngine, const wrl::ComPtr<ID3D12GraphicsCommandList> cmdList, CommandAllocator* alloc);

      ID3D12GraphicsCommandList* dx_cmdlist();

      void transition_buffer(ConstantBuffer* resource, ResourceState state);
      void transition_buffer(VertexBuffer* resource, ResourceState state);
      void transition_buffer(IndexBuffer* resource, ResourceState state);
      void transition_buffer(UploadBuffer* resource, ResourceState state);
      void transition_buffer(Texture2D* resource, ResourceState state);

      void update_buffer(ConstantBuffer* buffer, const void* data, rsl::memory_size size, s32 offset) override;
      void update_buffer(VertexBuffer* buffer, const void* data, rsl::memory_size size, s32 offset) override;
      void update_buffer(IndexBuffer* buffer, const void* data, rsl::memory_size size, s32 offset) override;
      void update_texture2d(rhi::Texture2D* texture, const void* data) override;

    protected:
      void platform_reset(rhi::CommandAllocator* alloc) override;

    private:
      DxCopyEngine* api_engine();
      void transition_buffer(Buffer* resource, ID3D12Resource* d3d_resource, ResourceState state);
      void update_buffer(rhi::Buffer* buffer, ID3D12Resource* d3dResource, const void* data, rsl::memory_size size, s32 offset);

    private:
      wrl::ComPtr<ID3D12GraphicsCommandList> m_cmd_list;

    };
  }
}