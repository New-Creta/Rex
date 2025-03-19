#include "defines.hlsl"

struct PS_INPUT
{
  float4 pos : SV_POSITION;
  float4 col : COLOR0;
  float2 uv  : TEXCOORD0;
};
SamplerState default_sampler : register(s0, RENDER_PASS_REGISTER_SPACE);
Texture2D default_texture : register(t0, RENDER_PASS_REGISTER_SPACE);
       
float4 main(PS_INPUT input) : SV_Target
{
  float4 out_col = input.col * default_texture.Sample(default_sampler, input.uv);
  return out_col; 
}