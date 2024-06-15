#pragma once

namespace rex
{
  namespace rhi
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