#pragma once

#include "rex_renderer_core/gfx/graphics_context.h"

#include "rex_std/bonus/memory.h"
#include "rex_engine/engine/types.h"

#include "rex_engine/diagnostics/assert.h"

#include "rex_renderer_core/system/view_heap.h"
#include "rex_renderer_core/gfx/view_heap_type.h"
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

      // Copy texture views into a heap that's accessible to shaders
      rsl::unique_ptr<ResourceView> copy_texture_views_to_shaders(const rsl::vector<ResourceView*>& views);
      // Copy sampler views into a heap that's accessible to shaders
      rsl::unique_ptr<ResourceView> copy_sampler_views_to_shaders(const rsl::vector<ResourceView*>& views);
      // Copy views into a heap that's accessible to shaders
      rsl::unique_ptr<ResourceView> copy_views(ViewHeapType heapType, const rsl::vector<ResourceView*>& views);

      // Update a constant buffer's data
      virtual void update_buffer(ConstantBuffer* buffer, const void* data, rsl::memory_size size, s32 offset) = 0;
      // Update a vertex buffer's data
      virtual void update_buffer(VertexBuffer* buffer, const void* data, rsl::memory_size size, s32 offset) = 0;
      // Update a index buffer's data
      virtual void update_buffer(IndexBuffer* buffer, const void* data, rsl::memory_size size, s32 offset) = 0;
      // Update a texture's data
      virtual void update_texture2d(Texture2D* texture, const void* data) = 0;

    protected:
      // Perform a reset, specific to the type of context
      void type_specific_reset(const ContextResetData& resetData) override;

    private:
      // Copy views into a heap that's accessible to shaders
      rsl::unique_ptr<ResourceView> copy_views(ViewHeap* dstHeap, const rsl::vector<ResourceView*>& descriptors);

    private:
      ViewHeap* m_shader_visible_srv_heap;      // the srv view heap that's accessible to shaders
      ViewHeap* m_shader_visible_sampler_heap;  // the sampler view heap that's accessible to shaders
    };
  }
}