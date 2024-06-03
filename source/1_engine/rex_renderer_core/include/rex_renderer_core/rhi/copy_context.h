#pragma once

#include "rex_renderer_core/rhi/graphics_context.h"
#include "rex_renderer_core/gfx/copy_engine.h"

#include "rex_std/bonus/memory.h"
#include "rex_engine/engine/types.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  namespace rhi
  {
    class Buffer;
    class Texture2D;

    class CopyContext : public GraphicsContext
    {
    public:
      CopyContext(gfx::GraphicsEngine* owningEngine);

      virtual void update_buffer(ConstantBuffer* buffer, const void* data, rsl::memory_size size, s32 offset) = 0;
      virtual void update_buffer(VertexBuffer* buffer, const void* data, rsl::memory_size size, s32 offset) = 0;
      virtual void update_buffer(IndexBuffer* buffer, const void* data, rsl::memory_size size, s32 offset) = 0;
      virtual void update_texture2d(rhi::Texture2D* texture, const void* data) = 0;

      // Copy Buffer
      // Copy Texture
      // Copy Resource (any that's not a buffer or a texture)
      // Copy Tiles
      // Transition Resource States

    protected:
      gfx::CopyEngine* typed_owning_engine()
      {
        return static_cast<gfx::CopyEngine*>(owning_engine());
      }

    private:

    };
  }
}