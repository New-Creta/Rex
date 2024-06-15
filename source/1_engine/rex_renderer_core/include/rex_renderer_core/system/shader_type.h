#pragma once

namespace rex
{
  namespace gfx
  {
    enum class ShaderType
    {
      None = 0,
      Vertex,   // Used for vertex shader
      Pixel,    // Used for pixel shader
      Geometry, // Used for geometry shader
      Compute   // Used for compute shader
    };
  } // namespace gfx
} // namespace rex