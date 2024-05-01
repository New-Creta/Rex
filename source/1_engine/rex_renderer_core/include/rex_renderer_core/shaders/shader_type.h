#pragma once

namespace rex
{
  namespace rhi
  {
    enum class ShaderType
    {
      None = 0,
      Vertex,
      Pixel,
      Geometry,
      Compute
    };
  } // namespace rhi
} // namespace rex