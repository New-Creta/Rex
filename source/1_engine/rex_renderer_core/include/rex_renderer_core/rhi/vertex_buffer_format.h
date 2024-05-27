#pragma once

namespace rex
{
  namespace renderer
  {
    // Graphics api abstraction of vertex buffer format
    enum class VertexBufferFormat
    {
      None = 0,
      Float1,
      Float2,
      Float3,
      Float4,
      UNorm4,
      UNorm2,
      UNorm1
    };
  } // namespace renderer
} // namespace rex