#include "defines.hlsl"
#include "utils.hlsl" // for is_bit_set

// Bit field indexes for the bit flags bellow in the per instance buffer
#define FLIP_X_BIT 0
#define FLIP_Y_BIT 1
#define RENDER_BOTTOM_BEHIND_BG 2

// Data to describe how big the render target is
// as well how we're mapping the vintage pixels onto it
cbuffer RenderingMetaData : register(b0, RENDER_PASS_REGISTER_SPACE)
{
  float2 inv_vintage_pixel_screen_size;      // the inverse size of a single vintage pixel on the render target
  float2 render_target_size;                 
};

// Information unique for every sprite
struct PerInstanceData
{
  // screen position, scaled in vintage pixel coordinates
  int2 position;
  
  // Sprite info
  float2 inv_sprite_screen_size;  // the inverse size of the sprite on the vintage screen
  float2 inv_sprite_texture_size; // the inverse size of the sprite within its texture

  // the start UV position for the current sprite
  // this allows the shader to select to correct sprite
  // from the texture with the same vertex UV information
  float2 uv_start;
  
  // flipping X or Y uv channels
  int bit_flags;
};

StructuredBuffer<PerInstanceData> instance_data : register(t0, RENDER_PASS_REGISTER_SPACE);

// Vertices expected for this shader are meant to span the entire screen
// as in, without any transforms, it will cover { -1, 1 } until { 1, -1 } of the window (NDC coordinates)
// This shader transforms the vertex positions and UVs to only span the size of the sprite on the vintage screen
// Meaning if a sprite is 16x16 pixels, it'll be shrunk down to represent 16x16 pixels
// However because we're rendering on a bigger render target than the vintage screen
// the 16x16 pixels will be grown so the span the same amount of space on the rendertarget
// as they would have on the old vintage screen

// =========================================
// VERTEX SHADER
// =========================================

struct VertexIn
{
  float3 PosL : POSITION;             // The position of the vertex in local space. This should always be one of the corners of the screen
  float2 Uv : TEXCOORD0;              
  uint InstanceId : SV_InstanceID;
};

struct VertexOut
{
  float4 PosH : SV_POSITION;
  float2 Uv : TEXCOORD0;
  uint InstanceId : SV_InstanceID;
};

float4 calculate_vertex_position(VertexIn vin)
{
  VertexOut vout;

  float2 pos = { -1.0f, 1.0f };                                               // start from the top left
  pos += vin.PosL.xy * instance_data[vin.InstanceId].inv_sprite_screen_size;  // scale down to position to its size relative to the render target

  // Offset the sprite onto the position where it should be drawn on screen
  pos.x += (instance_data[vin.InstanceId].position.x * inv_vintage_pixel_screen_size.x); // offset the vertex to where we want on screen
  pos.y -= (instance_data[vin.InstanceId].position.y * inv_vintage_pixel_screen_size.y); // offset the vertex to where we want on screen
    
  // Store the Z value as well, as it'll be used to draw the background over the sprite if required
  return float4(pos, vin.PosL.z, 1.0f);
}

float2 calculate_vertex_uv(VertexIn vin)
{
  // Add the offset to the original uv offset
  
  // flip the UVs based on the bits set in the bit mask
  // We do it this way so we need to have less data stored on disk and the GPU
  if (is_bit_set(instance_data[vin.InstanceId].bit_flags, FLIP_X_BIT))
  {
    vin.Uv.x = 1.0f - vin.Uv.x;
  }
  if (is_bit_set(instance_data[vin.InstanceId].bit_flags, FLIP_Y_BIT))
  {
    vin.Uv.y = 1.0f - vin.Uv.y;
  }
  
  float2 uv = { 0.0, 0.0 };
  
  // scale down the UV so it only ranges a single sprite instead of the entire texture
  uv += vin.Uv * instance_data[vin.InstanceId].inv_sprite_texture_size;
  
  // offset the UV so it starts at the top left position of the sprite we want to draw
  uv += instance_data[vin.InstanceId].uv_start;
  
  return uv;
}

VertexOut VSMain(VertexIn vin)
{
  VertexOut vout;
  
  vout.PosH = calculate_vertex_position(vin);
  vout.Uv = calculate_vertex_uv(vin);
  vout.InstanceId = vin.InstanceId;
  
  return vout;
}

// =========================================
// PIXEL SHADER
// =========================================

struct PS_INPUT
{
  float4 PosH : SV_POSITION;
  float2 Uv : TEXCOORD0;
  uint InstanceId : SV_InstanceID;
};

SamplerState default_sampler : register(s0, RENDER_PASS_REGISTER_SPACE);
Texture2D sprite_texture : register(t1, RENDER_PASS_REGISTER_SPACE);
Texture2D background_texture : register(t2, RENDER_PASS_REGISTER_SPACE);

// See https://asawicki.info/news_1516_half-pixel_offset_in_directx_11 for information about the half pixel rule
// the position input will always be the pixel idx value, with an offset of 0.5
// we need to convert this position back into screen space, so we divide it by the resolution

// What we could do as well is set the alpha value in the vertex shader if the tile rendered is a tile that may be transparent
// Then blending can be used to render the correct pixel or not

float4 PSMain(PS_INPUT pin) : SV_Target
{
  float2 uv_in_map = pin.PosH.xy / render_target_size;
  float3 white_color = { 1.0f, 1.0f, 1.0f };
 
  if (pin.PosH.z < 0.5f && is_bit_set(instance_data[pin.InstanceId].bit_flags, RENDER_BOTTOM_BEHIND_BG)) // only the bottom half of the sprite
  {
    float4 background_color = background_texture.Sample(default_sampler, uv_in_map);
    background_color.a = 1.0f;

    // check if the pixel in the background is white or not
    // if it is not white, discard the pixel, allowing the background to remain
    if (!is_equal(background_color.rgb, white_color))
    {
      discard;
    }
  }
  
  float4 texture_color = sprite_texture.Sample(default_sampler, pin.Uv);
  float4 final_color = float4(texture_color.rgb, 1.0f);
  
  // filter out white color, as this is meant to represent it's transparent
  if (is_equal(final_color.rgb, white_color))
  {
    final_color = float4(1.0f, 0.0f, 0.0f, 0.0f);
  }
    
  return final_color;
}


