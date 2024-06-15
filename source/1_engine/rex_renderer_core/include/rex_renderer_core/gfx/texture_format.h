#pragma once

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
  } // namespace gfx
} // namespace rex