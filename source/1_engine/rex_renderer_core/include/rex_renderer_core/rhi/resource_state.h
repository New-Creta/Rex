#pragma once

#include "rex_engine/engine/defines.h"

namespace rex
{
  namespace rhi
  {
    // A gpu resource can only be in a single resource state
    // Depending on the resource state, the driver can optimize where the resource is put in memory
    enum class ResourceState
    {
      Common                             = BIT(0),  // Resource state used for accessing a resource across graphics engine types
      VertexAndConstantBuffer            = BIT(1),  // for resources meant to be used as a vertex buffer or a constant buffer
      IndexBuffer                        = BIT(2),  // for resources meant to be used as an index buffer
      RenderTarget                       = BIT(3),  // for resources meant to be used as a render target
      UnorderedAccess                    = BIT(4),  // for resources meant to be used unordered access
      DepthWrite                         = BIT(5),  // for resources meant to be used as the target for depth writing
      DepthRead                          = BIT(6),  // for resources meant to be used as the target for depth reading (can be combined with others)
      NonPixelShaderResource             = BIT(7),  // for resources meant to be used with a shader that's not the pixel shader
      PixelShaderResource                = BIT(8),  // for resources meant to be used with the pixel shader
      StreamOut                          = BIT(9),  // for resources meant to be used with stream output
      IndirectArgument                   = BIT(10), // for resources used as an indirect argument
      CopyDest                           = BIT(11), // for resources used as copy destination
      CopySource                         = BIT(12), // for resources used as copy source
      ResolveDest                        = BIT(13), // for resources used as the destinaton for a resolve operation
      ResolveSource                      = BIT(14), // for resources used as the source for a resolve operation
      RaytracingAccelerationStructure    = BIT(15), // resources initialized in this state are meant for raytracing acceleration
      ShadingRateSource                  = BIT(16), // for resources used as screen-space shading rate image for variable-rate shading
      Predication                        = BIT(18), // for resources used for predication
      VideoDecodeRead                    = BIT(19), // for resources used as source for a decode operation
      VideoDecodeWrite                   = BIT(20), // for resources used as destination for a decoded operation
      VideoProcessRead                   = BIT(21), // for resources used to read video data during video processing
      VideoProcessWrite                  = BIT(22), // for resources used to write video processing during video processing
      VideoEncodeRead                    = BIT(23), // for resources used as the source in an encode operation
      VideoEncodeWrite                   = BIT(24), // for resources used as the destination in an encode operation
      GenericRead                        = VertexAndConstantBuffer | IndexBuffer | NonPixelShaderResource | PixelShaderResource | Predication | CopySource, // 

      Present = Common,
    };
  }
}