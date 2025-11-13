#include "defines.hlsl"

#define FLIP_X_BIT 0
#define FLIP_Y_BIT 1
#define RENDER_BOTTOM_BEHIND_BG 2

// Every constant buffer needs to be 256 byte aligned
// So we try to pack as much data together into 1 buffer
cbuffer RenderingMetaData : register(b0, RENDER_PASS_REGISTER_SPACE)
{
  // Render target data
  float2 inv_tile_screen_size;      // the inverse size of a single tile on the screen
  float2 screen_size; 

  // Sprite info
  float2 inv_sprite_screen_size;    // the inverse size of the sprite on the screen
  float2 inv_sprite_texture_size;     // the inverse of the size of a single tile in the texture

  // The offset in tiles from the top left of the screen
  // where this sprite should be drawn
  float2 top_left_offset;

  // SPRITE ANIMATION INFO
  // the start UV position for the current sprite
  // this allows the shader to select to correct sprite
  // from the texture
  float2 uv_start;

  // flipping X or Y uv channels
  int bit_flags;
};

// Vertices expected for this shader are meant to spawn the entire screen
// as in, without any transforms, it will cover { 0, 0 } until { 1, 1 } of the window
// This shader transforms the vertex positions and UVs to only spawn the size a single
// tile takes up in the render target and in the tileset texture

// =========================================
// VERTEX SHADER
// =========================================

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

bool is_bit_set(int mask, int bit)
{
  return (mask >> bit) & 0x01;
}

float4 calculate_vertex_position(VertexIn vin)
{
  VertexOut vout;

  // Calculate the position of this sprite
  float2 pos = { -1.0f, 1.0f };                            // start from the top left
  pos += vin.PosL.xy * inv_sprite_screen_size;             // scale down to position to its size relative to the render target

  pos.x += (top_left_offset.x * inv_tile_screen_size.x);   // offset the vertex to where we want on screen
  pos.y -= (top_left_offset.y * inv_tile_screen_size.y);   // offset the vertex to where we want on screen
    
  // Offset the position to this position
  return float4(pos, vin.PosL.z, 1.0f);
}

float2 calculate_vertex_uv(VertexIn vin)
{
  // Add the offset to the original uv offset
  if (is_bit_set(bit_flags, FLIP_X_BIT))
  {
    vin.Uv.x = 1.0f - vin.Uv.x;
  }
  if (is_bit_set(bit_flags, FLIP_Y_BIT))
  {
    vin.Uv.y = 1.0f - vin.Uv.y;
  }
  
  float2 uv = { 0.0, 0.0 };
  uv += vin.Uv * inv_sprite_texture_size;
  uv += uv_start;
  
  return uv;
}

VertexOut VSMain(VertexIn vin)
{
  VertexOut vout;
  
  vout.PosH = calculate_vertex_position(vin);
  vout.Uv = calculate_vertex_uv(vin);

  return vout;
}

// =========================================
// PIXEL SHADER
// =========================================

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
  float2 uv_in_map = pin.PosH.xy / screen_size;
 
  if (pin.PosH.z < 0.5f && is_bit_set(bit_flags, RENDER_BOTTOM_BEHIND_BG)) // only the bottom half of the sprite
  {
    float4 background_color = background_texture.Sample(default_sampler, uv_in_map);
    background_color.a = 1.0f;

    // calculate the pixel we're currently drawing
    // in the bg texture
    if ((background_color.r == 1.0f && background_color.g == 1.0f && background_color.b == 1.0f) == false)
    {
      return background_color;
    }    
  } 
  
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


