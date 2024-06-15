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

      // Return the current offset resource will be allocated in
      s64 current_offset() const;
      // Increase the offset by a given amount and return the old value
      s64 inc_offset(s64 size, s32 alignment);

    private:
      s64 m_offset;                                // the offset we're currently writing in
    };
  }
}