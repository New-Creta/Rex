#pragma once

namespace rex
{
  namespace gfx
  {
    enum class ShaderType
    {
      None = 0,
      Vertex,    // Used for vertex shader
      Pixel,     // Used for pixel shader
      Geometry,  // Used for geometry shader
      Compute,   // Used for compute shader
      Hull,      // Used for hull shader
      Domain,    // Used for domain shader
      Amplification,  // Used for amplification shader
      Mesh,      // Used for mesh shader
    };
  } // namespace gfx
} // namespace rex