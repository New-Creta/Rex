#include "defines.hlsl"

struct PS_INPUT
{
  float4 PosH : SV_POSITION;
  float2 Uv : TEXCOORD0;
};

SamplerState default_sampler : register(s0, RENDER_PASS_REGISTER_SPACE);
Texture2D tile_texture : register(t0, RENDER_PASS_REGISTER_SPACE);

float4 main(PS_INPUT pin) : SV_Target
{
  // As textures are not in sRGB color space, we have to convert them
  float4 texture_color = tile_texture.Sample(default_sampler, pin.Uv);

  // Apply the diffuse strength on the final color
  return float4(texture_color.rgb, 1.0f);
}


