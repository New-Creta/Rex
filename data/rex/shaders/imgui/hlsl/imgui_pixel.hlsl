struct PS_INPUT
{
  float4 pos : SV_POSITION;
  float4 col : COLOR0;
  float2 uv  : TEXCOORD0;
};
SamplerState fonts_sampler : register(s0);
Texture2D fonts_texture : register(t0);
       
float4 main(PS_INPUT input) : SV_Target
{
  float4 out_col = input.col * fonts_texture.Sample(fonts_sampler, input.uv); 
  return out_col; 
}