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
      AllShaderResources = ConstantBufferView,
      RenderTargetView,
      DepthStencilView,
      Sampler
    };
  }
}