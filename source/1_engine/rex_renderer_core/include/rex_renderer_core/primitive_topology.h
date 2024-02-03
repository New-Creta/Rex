#pragma once

namespace rex
{
  namespace renderer
  {
    enum class PrimitiveTopology
    {
      None = 0,
      POINTLIST,
      LINELIST,
      LINESTRIP,
      TRIANGLELIST,
      TRIANGLESTRIP
    };
  } // namespace renderer
} // namespace rex