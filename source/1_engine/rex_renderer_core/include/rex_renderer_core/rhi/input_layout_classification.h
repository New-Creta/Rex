#pragma once

namespace rex
{
  namespace renderer
  {
    // Graphics api abstraction of input layout classification
    enum class InputLayoutClassification
    {
      None = 0,
      PerVertexData,
      PerInstanceData
    };
  } // namespace renderer
} // namespace rex