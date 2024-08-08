// A constant buffer that holds data per viewport
cbuffer cbPerView
{
    float4x4 gView;
    float4x4 gProj;
    float4x4 gViewProj;
    float3 gLightDirection;
};

// A constant buffer that holds data per instance
cbuffer cbPerObject
{
  float4x4 gWorld;
  float4x4 gWorldViewProj;
};