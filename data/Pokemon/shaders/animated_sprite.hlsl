#include "defines.hlsl"

// This buffers holds the indices of each tile in the texture.
// Using this information, we can calculate the UV for each vertex
// Each index is only 1 byte, so you need to use bit shifting to unpack it
// TLDR: this maps instance id --> tile id in texture
ByteAddressBuffer TileIndexIntoTextureBuffer : register(t0, RENDER_PASS_REGISTER_SPACE);

// Every constant buffer needs to be 256 byte aligned
// So we try to pack as much data together into 1 buffer
cbuffer RenderingMetaData : register(b0, RENDER_PASS_REGISTER_SPACE)
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

// Vertices expected for this shader are meant to spawn the entire screen
// as in, without any transforms, it will cover { 0, 0 } until { 1, 1 } of the window
// This shader transforms the vertex positions and UVs to only spawn the size a single
// tile takes up in the render target and in the tileset texture

struct VertexIn
{
  float3 PosL : POSITION; // The position of the vertex in local space
  float2 Uv : TEXCOORD0; // The UV of the vertex
  uint VertexId : SV_VertexId;
};

struct VertexOut
{
  float4 PosH : SV_POSITION;
  float2 Uv : TEXCOORD0;
};

// Convert a index into a 2D coordinate.
uint2 index_to_coord(uint tileIdx, uint width)
{
  uint2 coord;

  coord.x = tileIdx % width;
  coord.y = tileIdx / width;

  return coord;
}

float4 calculate_vertex_position(VertexIn vin)
{
  VertexOut vout;

  // Calculate the position of this cell, starting from top left
  float2 pos = { -1.0f, 1.0f }; // start from the top left
  pos.x += vin.PosL.x * inv_sprite_screen_width; // scale down to position to its size relative to the render target
  pos.x += (8 * inv_tile_screen_width); // offset the vertex based on the current instance we're rendering
  pos.y += vin.PosL.y * inv_sprite_screen_height;
  pos.y -= (8 * inv_tile_screen_height);
  pos.y += 4 * (inv_tile_screen_height / 8);
    
  float z = vin.PosL.z + (player_is_on_grass * 1.0f);
  z = min(1.0f, z);
  // z = 0 * x + y == 1
  // z = 1 * x + y == 1
  // Offset the position to this position
  return float4(pos, vin.PosL.z, 1.0f);
}

float2 calculate_vertex_uv(VertexIn vin)
{
  // Add the offset to the original uv offset
  
  if (flip_x)
  {
    vin.Uv.x = 1.0f - vin.Uv.x;
  }
  if (flip_y)
  {
    vin.Uv.y = 1.0f - vin.Uv.y;
  }
  
  float2 uv = { 0.0, 0.0 };
  uv.x += vin.Uv.x * inv_tile_texture_width;
  uv.x += uv_start_x;
  uv.y += vin.Uv.y * inv_tile_texture_height;
  uv.y += uv_start_y;
  
  return uv;
}

VertexOut VSMain(VertexIn vin)
{
  VertexOut vout;
  
  vout.PosH = calculate_vertex_position(vin);
  vout.Uv = calculate_vertex_uv(vin);

  return vout;
}

struct PS_INPUT
{
  float4 PosH : SV_POSITION;
  float2 Uv : TEXCOORD0;
};

SamplerState default_sampler : register(s0, RENDER_PASS_REGISTER_SPACE);
Texture2D sprite_texture : register(t0, RENDER_PASS_REGISTER_SPACE);
Texture2D background_texture : register(t1, RENDER_PASS_REGISTER_SPACE);

// See https://asawicki.info/news_1516_half-pixel_offset_in_directx_11 for information about the half pixel rule
// the position input will always be the pixel idx value, with an offset of 0.5
// we need to convert this position back into screen space, so we divide it by the resolution

float4 PSMain(PS_INPUT pin) : SV_Target
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


