struct VertexOut
{
  float4 PosH : SV_POSITION;
  float4 Color : COLOR;
  float3 normal : NORMAL;
};

float4 PS(VertexOut pin) : SV_Target
{
  float3 light_direction = float3(-0.577f, -0.577f, 0.577f);

  float diffuseStrength = dot(pin.normal, -light_direction);
  diffuseStrength = saturate(diffuseStrength);

  float3 color_rgb = pin.Color.rgb * diffuseStrength;

  return float4(color_rgb, 1.0f);
}

