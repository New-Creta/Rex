#pragma once

namespace rex
{
  namespace gfx
  {
    enum class ViewHeapType
    {
      ConstantBuffer   = 1,  // Constant buffers, unordered access buffers and textures are sharing the same view heap
      Texture2D        = 1,  // Constant buffers, unordered access buffers and textures are sharing the same view heap
      Texture3D        = 1,  // Constant buffers, unordered access buffers and textures are sharing the same view heap
      TextureCube      = 1,  // Constant buffers, unordered access buffers and textures are sharing the same view heap
      ByteAddress      = 1,  // Constant buffers, unordered access buffers and textures are sharing the same view heap
      UnorderedAccess  = 1,  // Constant buffers, unordered access buffers and textures are sharing the same view heap
      RenderTarget     = 2,
      DepthStencil     = 3,
      Sampler          = 4
    };
  }
}