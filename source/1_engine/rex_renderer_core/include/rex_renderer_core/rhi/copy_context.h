#pragma once

#include "rex_renderer_core/rhi/graphics_context.h"

namespace rex
{
  namespace rhi
  {
    class CopyContext : public GraphicsContext
    {
    public:
      void update_buffer(rhi::Buffer* buffer, const void* data, rsl::memory_size size, s32 offset);
      void wait_for_finish();

      // Copy Buffer
      // Copy Texture
      // Copy Resource (any that's not a buffer or a texture)
      // Copy Tiles
      // Transition Resource States

    private:

    };
  }
}