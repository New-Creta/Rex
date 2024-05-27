#pragma once

namespace rex
{
  namespace renderer
  {
    // Graphics Api abstraction of the cull mode
    enum class CullMode
    {
      None,
      Front,
      Back
    };
  } // namespace renderer
} // namespace rex