#pragma once

namespace rex
{
  namespace rhi
  {
    enum class ShaderType
    {
      None = 0,
      VERTEX,
      PIXEL,
      GEOMETRY,
      COMPUTE
    };
  } // namespace rhi
} // namespace rex