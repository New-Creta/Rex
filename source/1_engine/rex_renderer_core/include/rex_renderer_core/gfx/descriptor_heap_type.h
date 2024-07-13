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
      ShaderResources = ConstantBufferView,
      RenderTargetView,
      DepthStencilView,
      Sampler
    };
  }
}