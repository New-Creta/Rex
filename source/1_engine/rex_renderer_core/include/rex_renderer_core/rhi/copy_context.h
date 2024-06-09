#pragma once

#include "rex_renderer_core/rhi/graphics_context.h"

#include "rex_std/bonus/memory.h"
#include "rex_engine/engine/types.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  namespace gfx
  {
    class CopyEngine;
  }

  namespace rhi
  {
    class Buffer;
    class ConstantBuffer;
    class VertexBuffer;
    class IndexBuffer;
    class Texture2D;

    class CopyContext : public GraphicsContext
    {
    public:
      CopyContext(gfx::GraphicsEngine* owningEngine);

      virtual void update_buffer(ConstantBuffer* buffer, const void* data, rsl::memory_size size, s32 offset) = 0;
      virtual void update_buffer(VertexBuffer* buffer, const void* data, rsl::memory_size size, s32 offset) = 0;
      virtual void update_buffer(IndexBuffer* buffer, const void* data, rsl::memory_size size, s32 offset) = 0;
      virtual void update_texture2d(rhi::Texture2D* texture, const void* data) = 0;

    protected:
      gfx::CopyEngine* typed_owning_engine();
    };
  }
}