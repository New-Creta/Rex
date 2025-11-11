#include "defines.hlsl"

struct VertexIn
{
  float2 PosL : POSITION; // The position of the vertex in local space
  float2 Uv : TEXCOORD0; // The UV of the vertex
};

struct VertexOut
{
  float4 PosH : SV_POSITION;
  float2 Uv : TEXCOORD0;
};

VertexOut main(VertexIn vin)
{
  VertexOut vout;
  
  vout.PosH = float4(vin.PosL, 0.0f, 1.0f);
  vout.Uv = vin.Uv;

  return vout;
}


