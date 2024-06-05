#pragma once

namespace rex
{
  namespace rhi
  {
    enum class ResourceState
    {
      Common,
      VertexAndConstantBuffer,
      IndexBuffer,
      RenderTarget,
      UnorderedAccess,
      DepthWrite,
      DepthRead,
      NonPixelShaderResource,
      PixelShaderResource,
      StreamOut,
      IndirectArgument,
      CopyDest,
      CopySource,
      ResolveDest,
      ResolveSource,
      RaytracingAccelerationStructure,
      ShadingRateSource,
      GenericRead,
      Predication,
      VideoDecodeRead,
      VideoDecodeWrite,
      VideoProcessRead,
      VideoProcessWrite,
      VideoEncodeRead,
      VideoEncodeWrite,

      Present = 0,
    };
  }
}