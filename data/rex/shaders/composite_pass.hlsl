#include "defines.hlsl"

// This shader simply takes in a set of vertices without transforming them
// and samples a texture on top of them
// This is useful for copying a render target to the backbuffer
// but can be used for other features as well

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

VertexOut VSMain(VertexIn vin)
{
  VertexOut vout;
  
  vout.PosH = float4(vin.PosL, 0.0f, 1.0f);
  vout.Uv = vin.Uv;

  return vout;
}

struct PS_INPUT
{
  float4 PosH : SV_POSITION;
  float2 Uv : TEXCOORD0;
};

SamplerState default_sampler : register(s0, RENDER_PASS_REGISTER_SPACE);
Texture2D src_texture : register(t0, RENDER_PASS_REGISTER_SPACE);

float4 PSMain(PS_INPUT pin) : SV_Target
{
  // As textures are not in sRGB color space, we have to convert them
  float4 texture_color = src_texture.Sample(default_sampler, pin.Uv);

  // Apply the diffuse strength on the final color
  float4 color = float4(texture_color.rgb, 1.0f);
  
  return color;
}


