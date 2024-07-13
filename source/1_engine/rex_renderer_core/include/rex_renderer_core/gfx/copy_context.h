#pragma once

#include "rex_renderer_core/gfx/graphics_context.h"

#include "rex_std/bonus/memory.h"
#include "rex_engine/engine/types.h"

#include "rex_engine/diagnostics/assert.h"

#include "rex_renderer_core/system/descriptor_heap.h"
#include "rex_renderer_core/system/resource_view.h"

namespace rex
{
  namespace gfx
  {
    class CopyEngine;
  }

  namespace gfx
  {
    class ConstantBuffer;
    class VertexBuffer;
    class IndexBuffer;
    class Texture2D;

    // A copy context is used for copying commands but cannot be used for other commands
    class CopyContext : public GraphicsContext
    {
    public:
      CopyContext(gfx::GraphicsEngine* owningEngine);

      virtual rsl::unique_ptr<ResourceView> copy_descriptors(DescriptorHeap* srcHeap, const rsl::vector<ResourceView*>& descriptors) = 0;

      // Update a constant buffer's data
      virtual void update_buffer(ConstantBuffer* buffer, const void* data, rsl::memory_size size, s32 offset) = 0;
      // Update a vertex buffer's data
      virtual void update_buffer(VertexBuffer* buffer, const void* data, rsl::memory_size size, s32 offset) = 0;
      // Update a index buffer's data
      virtual void update_buffer(IndexBuffer* buffer, const void* data, rsl::memory_size size, s32 offset) = 0;
      // Update a texture's data
      virtual void update_texture2d(Texture2D* texture, const void* data) = 0;

      DescriptorHeap* global_srv_heap();
      DescriptorHeap* shader_visible_srv_heap();

    protected:
      void engine_reset(const ContextResetData& resetData) override;

    private:
      DescriptorHeap* m_global_srv_heap;
      DescriptorHeap* m_shader_visible_srv_heap;
    };
  }
}