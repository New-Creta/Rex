#pragma once

namespace rex
{
  namespace gfx
  {
    enum class DescriptorHeapType
    {
      ConstantBufferView,
      ShaderResourceView = ConstantBufferView,
      UnorderedAccessView = ConstantBufferView,
      RenderTargetView,
      DepthStencilView,
      Sampler
    };
  }
}