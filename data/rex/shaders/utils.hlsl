// Convert a color from linear color space to sRGB
float4 convert_to_srgb(float4 color)
{
  color.r = pow((color.r + 0.055f) / 1.055f, 2.4f);
  color.g = pow((color.g + 0.055f) / 1.055f, 2.4f);
  color.b = pow((color.b + 0.055f) / 1.055f, 2.4f);

  return color;
}