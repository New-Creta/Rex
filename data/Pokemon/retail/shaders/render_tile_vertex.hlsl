#include "defines.hlsl"

struct VertexIn
{
  float2 PosL : POSITION;             // The position of the vertex in local space
  float2 Uv : TEXCOORD0;              // The UV of the vertex

  float4x4 WVP : InstMatrix;         // The WVP matrix of the vertex
};

struct VertexOut
{
  float4 PosH : SV_POSITION;
  float2 Uv : TEXCOORD0;
};

VertexOut main(VertexIn vin)
{
  VertexOut vout;

  // Transform to homogeneous clip space.
  vout.PosH = mul(float4(vin.PosL, 0.0f, 1.0f), vin.WVP);

  // Pass the other values straight to the pixel shader
  vout.Uv = vin.Uv;

  return vout;
}


