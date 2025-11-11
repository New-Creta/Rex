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
  // pin.PosH.x is of a rangbe between 0.5 and (width - 1) + 0.5
  // pin.PosH.y is of a rangbe between 0.5 and (height - 1) + 0.5
  
  float2 uv_in_map = { pin.PosH.x + 0.5f, pin.PosH.y + 0.5f };
  uv_in_map.r = uv_in_map.x / 800.0f;
  uv_in_map.g = uv_in_map.y / 720.0f;
  //uv_in_map.r *= 0.00001f;
  

  //background_color.y = 1.0f;
  
  if (pin.PosH.z < 0.5f && player_is_on_grass > 0.5f) // only the bottom half of the sprite
  {
    float4 background_color = background_texture.Sample(default_sampler, uv_in_map);
    background_color.a = 1.0f;
    // calculate the pixel we're currently drawing
    // in the bg texture

    //float2 uv_in_map  =  {0.0f, 0.0f};
    //uv_in_map.r = pin.PosH.x / 2 + 0.5f;
    //uv_in_map.g = pin.PosH.y / 2 + 0.5f;
    //uv_in_map.r *= 0.00001f;
    
    //uv_in_map.r = 0.25f;
    //return background_color;
    //background_color = background_texture.Sample(default_sampler, uv_in_map);
    //return background_color;
    //if (background_color.r != 1.0f || background_color.g != 1.0f || background_color.b != 1.0f)
    //{
    //  return float4(1.0f, 0.0f, 0.0f, 1.0f);
    //}

    if ((background_color.r == 1.0f && background_color.g == 1.0f && background_color.b == 1.0f) == false)
    {
      return background_color;
    }
    
    
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
    
  return color;
}


