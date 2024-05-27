#pragma once

namespace rex
{
  namespace renderer
  {
    // Graphics API only offers a limited set of colors for a static sampler
    enum class BorderColor
    {
      TransparentBlack, // Transparent black, meaning you'll use whatever color is behind it
      OpaqueBlack, // Opaque black, meaning you'll just use black, without caring what's behind it
      OpaqueWhite  // Opaque white, meaning you'll just use white, without caring what's behind it
    };
  }
}