#pragma once

namespace rex
{
  namespace renderer
  {
    enum class ShaderType
    {
      None = 0,
      VERTEX,
      PIXEL,
      GEOMETRY,
      COMPUTE
    };
  } // namespace renderer
} // namespace rex