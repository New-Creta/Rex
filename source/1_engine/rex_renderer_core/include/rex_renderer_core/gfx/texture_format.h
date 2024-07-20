#pragma once

#include "rex_engine/engine/invalid_object.h"

namespace rex
{
  namespace gfx
  {
    // Graphics api abstraction of texture formats
    enum class TextureFormat
    {
      None = 0,
      Unorm4,       // 4 components, 1 byte per component, normalized between 0 and 1
      Unorm4Srgb,   // 4 components, 1 byte per component, normalized between 0 and 1, scaled for SRGB
    };

    // Get the byte size of a given format
    constexpr s32 format_byte_size(TextureFormat format)
    {
      switch (format)
      {
      case rex::gfx::TextureFormat::Unorm4:       return 4 * sizeof(char8);
      case rex::gfx::TextureFormat::Unorm4Srgb:   return 4 * sizeof(char8);
      default: break;
      }

      return invalid_obj<s32>();
    }

  } // namespace gfx
} // namespace rex