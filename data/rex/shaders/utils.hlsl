// Convert a color from linear color space to sRGB
float4 convert_to_srgb(float4 color)
{
  color.r = pow((color.r + 0.055f) / 1.055f, 2.4f);
  color.g = pow((color.g + 0.055f) / 1.055f, 2.4f);
  color.b = pow((color.b + 0.055f) / 1.055f, 2.4f);

  return color;
}

// Check is a certain bit, counting from the least significat bit
// is set in an integer
bool is_bit_set(int mask, int bit)
{
  return (mask >> bit) & 0x01;
}

// compare 2 float3s and return if they're equal or not
// this is useful to do for an if statement
// as you can't compare colors directly in there
bool is_equal(float3 lhs, float3 rhs)
{
  float dist = distance(lhs, rhs);
  return (dist <= 0.001f) ? true : false;
}