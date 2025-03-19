#pragma once

namespace rex
{
  namespace gfx
  {
    enum class GraphicsEngineType
    {
      Render, // Type used to specify for render commands
      Copy,   // Type used to specify for copy commands
      Compute // Type used to specify for compute commands
    };

    using GraphicsContextType = GraphicsEngineType;
  }
}