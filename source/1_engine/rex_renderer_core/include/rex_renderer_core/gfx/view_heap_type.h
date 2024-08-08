#pragma once

namespace rex
{
  namespace gfx
  {
    enum class ViewHeapType
    {
      ConstantBuffer   = 0,
      Texture2D        = 1,  // Textures are sharing the same view heap
      Texture3D        = 1,  // Textures are sharing the same view heap
      TextureCube      = 1,  // Textures are sharing the same view heap
      RenderTarget     = 2,
      DepthStencil     = 3,
      Sampler          = 4
    };
  }
}