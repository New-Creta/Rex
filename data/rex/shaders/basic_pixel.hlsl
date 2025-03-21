#include "defines.hlsl"
#include "utils.hlsl"

cbuffer SceneData : register(b0, RENDER_PASS_REGISTER_SPACE) 
{
    float3 gLightDirection;
};

struct PS_INPUT
{
  float4 PosH : SV_POSITION;
  float3 Normal : NORMAL;
  float2 Uv : TEXCOORD0;
};
SamplerState default_sampler : register(s0, MATERIAL_REGISTER_SPACE);
Texture2D default_texture : register(t0, MATERIAL_REGISTER_SPACE);

float4 main(PS_INPUT pin) : SV_Target
{
  // As textures are not in sRGB color space, we have to convert them
  float4 texture_color = default_texture.Sample(default_sampler, pin.Uv);
  texture_color = convert_to_srgb(texture_color);

  // Calculate the diffuse strength based on the light direction
  float diffuseStrength = dot(pin.Normal, -gLightDirection);
  diffuseStrength = saturate(diffuseStrength);

  // Apply the diffuse strength on the final color
  return float4(texture_color.rgb * diffuseStrength, texture_color.a);
}


