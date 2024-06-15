#pragma once

namespace rex
{
  namespace gfx
  {
    enum class PrimitiveTopology
    {
      None = 0,
      PointList,    // A list of points
      LineList,     // A list of isolated straight line segments
      LineStrip,    // A primitive composed of connected line segments
      TriangleList, // A primitive of isolated triangles
      TriangleStrip // A primitive of connected triangles
    };
  } // namespace gfx
} // namespace rex