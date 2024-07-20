#pragma once

#include "rex_renderer_core/gfx/graphics.h"
#include "rex_renderer_core/gfx/copy_context.h"
#include "rex_renderer_core/system/copy_engine.h"

#include "rex_directx/utility/dx_util.h"

namespace rex
{
  namespace gfx
  {
    class CommandAllocator;
    class DxCopyEngine;

    class DxCopyContext : public CopyContext
    {
    public:
      DxCopyContext(gfx::GraphicsEngine* owningEngine, const wrl::ComPtr<ID3D12GraphicsCommandList> cmdList);

      // Return the wrapped directx commandlist object
      ID3D12GraphicsCommandList* dx_cmdlist();

      // Transition a constant buffer to a new resource state
      void transition_buffer(ConstantBuffer* resource, ResourceState state);
      // Transition a vertex buffer to a new resource state
      void transition_buffer(VertexBuffer* resource, ResourceState state);
      // Transition a index buffer to a new resource state
      void transition_buffer(IndexBuffer* resource, ResourceState state);
      // Transition a upload buffer to a new resource state
      void transition_buffer(UploadBuffer* resource, ResourceState state);
      // Transition a texture to a new resource state
      void transition_buffer(Texture2D* resource, ResourceState state);

      // Update a constant buffer's data on the gpu
      void update_buffer(ConstantBuffer* buffer, const void* data, rsl::memory_size size, s32 offset) override;
      // Update a vertex buffer's data on the gpu
      void update_buffer(VertexBuffer* buffer, const void* data, rsl::memory_size size, s32 offset) override;
      // Update a index buffer's data on the gpu
      void update_buffer(IndexBuffer* buffer, const void* data, rsl::memory_size size, s32 offset) override;
      // Update a texture's data on the gpu
      void update_texture2d(Texture2D* texture, const void* data) override;

    protected:
      // Reset this context by resetting the commandlist and its allocator
      // Also bind the descriptor heap
      void platform_reset(CommandAllocator* alloc, const ContextResetData& resetData) override;

      // profiling events
      void begin_profile_event(rsl::string_view eventName) override;
      void end_profile_event() override;

    private:
      // Return the graphics engine casted into the directx class
      DxCopyEngine* api_engine();
      // Transition a buffer object into a new resource state
      void transition_buffer(Resource* resource, ID3D12Resource* d3d_resource, ResourceState state);
      // Update a buffer on the gpu
      void update_buffer(ID3D12Resource* d3dResource, const void* data, rsl::memory_size size, s32 offset);

    private:
      wrl::ComPtr<ID3D12GraphicsCommandList> m_cmd_list;
    };
  }
}