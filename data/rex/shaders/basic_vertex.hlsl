cbuffer cbPerObject : register(b0)
{
  float4x4 gWorld;
  float4x4 gWorldViewProj;
};

cbuffer cbView : register(b1)
{
  float4x4 gView;
  float4x4 gProj;
  float4x4 gViewProj;
};

struct VertexIn
{
  float3 PosL : POSITION;
  float3 Normal : NORMAL;
  float4 Color : COLOR;
};

struct VertexOut
{
  float4 PosH : SV_POSITION;
  float4 Color : COLOR;
  float3 normal : NORMAL;
};

VertexOut main(VertexIn vin)
{
  VertexOut vout;

  // Transform to homogeneous clip space.
  float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
  vout.PosH = mul(posW, gViewProj);

  vout.normal = normalize(mul(vin.Normal, (float3x3)gWorld));

  // Just pass vertex color into the pixel shader.
  vout.Color = vin.Color;

  return vout;
}


