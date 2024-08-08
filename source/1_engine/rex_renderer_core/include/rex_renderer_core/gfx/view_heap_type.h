#pragma once

namespace rex
{
  namespace gfx
  {
    enum class ViewHeapType
    {
      ConstantBufferView,
      ShaderResourceView = ConstantBufferView,
      UnorderedAccessView = ConstantBufferView,
      ResourceView = ConstantBufferView,
      RenderTargetView,
      DepthStencilView,
      Sampler
    };
  }
}