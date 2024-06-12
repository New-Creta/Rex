#pragma once

namespace rex
{
  namespace renderer
  {
    // Graphics api abstraction of input layout classification
    enum class InputLayoutClassification
    {
      None = 0,
      PerVertexData,  // Input data is per-vertex data
      PerInstanceData // Input data is per-instance data
    };
  } // namespace renderer
} // namespace rex