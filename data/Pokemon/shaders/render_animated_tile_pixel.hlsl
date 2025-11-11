#include "defines.hlsl"

struct PS_INPUT
{
  float4 PosH : SV_POSITION;
  float2 Uv : TEXCOORD0;
};

cbuffer RenderingMetaData2 : register(b1, RENDER_PASS_REGISTER_SPACE)
{
  // Render target data
  float inv_tile_screen_width; // the inverse of the width of a single tile on the screen
  float inv_tile_screen_height; // the inverse of the height of a single tile on the screen

  float inv_sprite_screen_width;
  float inv_sprite_screen_height;

  // Tile texture data
  float inv_tile_texture_width; // the inverse of the width of a single tile in the texture
  float inv_tile_texture_height; // the inverse of the height of a single tile in the texture

  float uv_start_x;
  float uv_start_y;
  float player_is_on_grass;
  
  bool flip_x;
  bool flip_y;
  
};

SamplerState default_sampler : register(s0, RENDER_PASS_REGISTER_SPACE);
Texture2D sprite_texture : register(t0, RENDER_PASS_REGISTER_SPACE);
Texture2D background_texture : register(t1, RENDER_PASS_REGISTER_SPACE);

float4 main(PS_INPUT pin) : SV_Target
{
  float4 background_color = float4(1.0f, 1.0f, 1.0f, 1.0f);

  //background_color.y = 1.0f;
  background_color.a = 1.0f;
  
  if (pin.PosH.z - player_is_on_grass == -1.0f) // only the bottom half of the sprite
  {
    // calculate the pixel we're currently drawing
    // in the bg texture

    float2 uv_in_map  =  {0.0f, 0.0f};
    uv_in_map.r = pin.PosH.x / 2 + 0.5f;
    uv_in_map.g = pin.PosH.y / 2 + 0.5f;
    uv_in_map.r *= 0.00001f;
    
    //uv_in_map.r = 0.25f;
    return float4(uv_in_map, 0.0f, background_color.a);
    //background_color = background_texture.Sample(default_sampler, uv_in_map);
    //return background_color;
    //if (background_color.r != 1.0f || background_color.g != 1.0f || background_color.b != 1.0f)
    //{
    //  return float4(1.0f, 0.0f, 0.0f, 1.0f);
    //}
    
    
    //background_color.r = 1.0f;
    //return background_color;
    ////background_color.r = 0.5f;
    //background_color.g = 0.5f;
    //background_color.b = 0.5f;

    //return background_color;
    //if (background_color.r != 1.0f || background_color.g == 1.0f || background_color.b == 1.0f)
    //{
    //  return float4(1.0f, 0.0f, 0.0f, 1.0f);
    //}
  }
  //else
  //{
  //  background_color.r = 1.0f;
  //  //background_color.g = 1.0f;
  //  background_color.b = 1.0f;    
  //}
  
  
  // As textures are not in sRGB color space, we have to convert them
  float4 texture_color = sprite_texture.Sample(default_sampler, pin.Uv);

  // Apply the diffuse strength on the final color
  float4 color = float4(texture_color.rgb, 1.0f);
  
  // filter out white color channel
  if (color.r == 1.0f && color.g == 1.0f && color.b == 1.0f)
  {
    color = float4(1.0f, 0.0f, 0.0f, 0.0f);
  }
    
  return background_color * color;
}


