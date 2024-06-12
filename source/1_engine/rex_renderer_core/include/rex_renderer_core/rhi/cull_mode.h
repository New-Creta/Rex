#pragma once

namespace rex
{
  namespace renderer
  {
    // Graphics Api abstraction of the cull mode
    enum class CullMode
    {
      None,    // Always cull all triangles
      Front,   // Do not draw triangles that are front facing
      Back     // Do not draw triangles that are back facing
    };
  } // namespace renderer
} // namespace rex