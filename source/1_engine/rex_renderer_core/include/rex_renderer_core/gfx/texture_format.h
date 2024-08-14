#pragma once

#include "rex_engine/engine/invalid_object.h"

namespace rex
{
  namespace gfx
  {
    // Graphics api abstraction of texture formats
    enum class TextureFormat
    {
      Unknown = 0,
      Unorm4,       // 4 components, 1 byte per component, normalized between 0 and 1
      Unorm4Srgb,   // 4 components, 1 byte per component, normalized between 0 and 1, scaled for SRGB
      Depth32,      // 1 components, 4 bytes for depth
      Depth24Stencil8 // 2 components, 3 bytes for depth, 1 byte for stencil
    };

    // Get the byte size of a given format
    s32 format_byte_size(TextureFormat format);

  } // namespace gfx
} // namespace rex