#pragma once

namespace rex
{
  namespace gfx
  {
    // Identifies a technique for resolving texture coordinates that are outside of the boundaries of a texture
    enum class TextureAddressMode
    {
      Wrap,       // Tile the texture at every integer junction
      Mirror,     // Flip the texture at every integer junction
      Clamp,      // texture coordinates outside of the [0.0, 1.0] are set to their limit, 0.0 or 1.0 respectively.
      Border,     // Texture coordinates outside of range [0.0, 1.0] are set to the border color specified in the sampler or directly in the shader
      MirrorOnce  // Takes tha absolute value value of thetexture coordinates (thus, mirroring around 0), and then clamps to the maximum value.
    };
  }
}