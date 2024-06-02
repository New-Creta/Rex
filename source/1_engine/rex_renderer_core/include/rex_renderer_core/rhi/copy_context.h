#pragma once

#include "rex_renderer_core/rhi/graphics_context.h"

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
      CopyContext()
        : GraphicsContext(CommandType::Copy)
      {

      }

      void update_buffer(rhi::Buffer* buffer, const void* data, rsl::memory_size size, s32 offset)
      {
        REX_ASSERT("Not implement yet");
      }
      void update_texture2d(rhi::Texture2D* texture, const void* data)
      {
        REX_ASSERT("Not implement yet");
      }

      // Copy Buffer
      // Copy Texture
      // Copy Resource (any that's not a buffer or a texture)
      // Copy Tiles
      // Transition Resource States

    private:

    };
  }
}