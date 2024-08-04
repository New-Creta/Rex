#include "defines.hlsl"

cbuffer SceneData : register(b0, RENDER_PASS_REGISTER_SPACE) 
{
    float3 gLightDirection;
};

struct VertexOut
{
  float4 PosH : SV_POSITION;
  float4 Color : COLOR;
  float3 normal : NORMAL;
};

float4 main(VertexOut pin) : SV_Target
{
  float diffuseStrength = dot(pin.normal, -gLightDirection);
  diffuseStrength = saturate(diffuseStrength + 0.3f);
  float3 color_rgb = pin.Color.rgb * diffuseStrength;

  return float4(color_rgb, 1.0f);
}


