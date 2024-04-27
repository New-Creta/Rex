cbuffer cbPerObject : register(b0)
{
  float4x4 gWorld;
};

cbuffer cbPass : register(b1)
{
  float4x4 gView;
  float4x4 gInvView;
  float4x4 gProj;
  float4x4 gInvProj;
  float4x4 gViewProj;
  float4x4 gInvViewProj;
  float3 gEyePosW;
  float cbPerObjectPad1;
  float2 gRenderTargetSize;
  float2 gInvRenderTargetSize;
  float gNearZ;
  float gFarZ;
  float gDeltaTime;
};

struct VertexIn
{
  float3 PosL : POSITION;
  float3 NormalL : NORMAL;
  float4 Color : COLOR;
};

struct VertexOut
{
  float4 PosH : SV_POSITION;
  float4 Color : COLOR;
  float3 normal : NORMAL;
};

VertexOut VS(VertexIn vin)
{
  VertexOut vout;

  // Transform to homogeneous clip space.
  float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
  vout.PosH = mul(posW, gViewProj);

  vout.normal = normalize(mul(vin.NormalL, (float3x3)gWorld));

  // Just pass vertex color into the pixel shader.
  vout.Color = vin.Color;

  return vout;
}


