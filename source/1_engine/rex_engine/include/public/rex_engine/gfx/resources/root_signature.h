#pragma once

#include "rex_engine/gfx/system/shader_type.h"

#include "rex_engine/engine/types.h"

namespace rex
{
  namespace gfx
  {
    // A root signature holds all the parameters for every shader type bound to a pipeline.
    // It only specifies the type, not the actual data
    // for example:
    //
    // -- VERTEX SHADER --
    //   
    // cbuffer my_vertex_buffer : register(b0)
    // {
    //    .. some data here ..
    // };
    //
    // .. other vertex shader code ..
    //
    // 
    // 
    // -- PIXEL SHADER -- 
    //
    // SamplerState sampler0 : register(s0);
    // Texture2D texture0 : register(t0);
    //
    // .. other pixel shader code ..
    //
    //
    // The above would result in a root signature as followed
    // [0] - constant buffer view - register 0 - vertex visibility
    // [1] - shader resource view - register 0 - pixel visibility
    // [2] - sampler              - register 0 - pixel visibility
    //
    class RootSignature
    {
    public:
      virtual ~RootSignature() = default;
    };
  }
}