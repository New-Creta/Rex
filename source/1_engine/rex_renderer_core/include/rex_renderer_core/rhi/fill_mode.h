#pragma once

namespace rex
{
  namespace renderer
  {
    // Graphics api abstraction of the fill mode
    enum class FillMode
    {
      Solid,      // Fill the triangles formed by vertices
      Wireframe   // Draw lines connecting the vertices
    };
  } // namespace renderer
} // namespace rex