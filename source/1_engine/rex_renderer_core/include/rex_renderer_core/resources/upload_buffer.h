#pragma once

#include "rex_renderer_core/resources/buffer.h"
#include "rex_engine/engine/types.h"
#include "rex_std/bonus/memory.h"

namespace rex
{
  namespace gfx
  {
    // Interface for an api agnostic upload buffer
    class UploadBuffer : public Buffer
    {
    public:
      explicit UploadBuffer(rsl::memory_size size);

      // Reset the offset back to 0 so new allocations would happen at the start
      void reset();

    protected:
      // Return if the buffer can support an allocation of a given size and allignment
      bool can_fit_alloc(rsl::memory_size allocSize, s32 alignment);

      // Return the offset buffer the last buffer data was written to
      s64 current_buffer_offset() const;
      // Return the offset the next texture data can go to
      s64 current_texture_offset() const;

      // Decrease the offset of where buffer data goes
      s64 dec_buffer_offset(s64 size);
      // Increase the offset of where texture data goes
      s64 inc_texture_offsset(s64 size, s32 alignment);

    private:
      // Textures are written at the front of the buffer
      // other buffers that don't need alignment are written at the back
      // This is to avoid unused padding space within an upload buffer
      // if buffers and textures are added in random order
      s64 m_texture_write_offset;
      s64 m_buffer_write_offset;
    };
  }
}