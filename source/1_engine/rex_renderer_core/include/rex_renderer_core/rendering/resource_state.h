#pragma once

namespace rex
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
    Present,    
    Predication,    
    VideoDecodeRead,    
    VideoDecodeWrite,    
    VideoProcessRead,    
    VideoProcessWrite,    
    VideoEncodeRead,    
    VideoEncodeWrite
  };
}