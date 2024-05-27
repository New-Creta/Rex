#pragma once

namespace rex
{
  namespace renderer
  {
    enum class PrimitiveTopology
    {
      None = 0,
      PointList,
      LineList,
      LineStrip,
      TriangleList,
      TriangleStrip
    };
  } // namespace renderer
} // namespace rex