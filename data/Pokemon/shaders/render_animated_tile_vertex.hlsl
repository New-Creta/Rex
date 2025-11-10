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
  return float4(pos, z, 1.0f);
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

VertexOut main(VertexIn vin)
{
  VertexOut vout;
  
  vout.PosH = calculate_vertex_position(vin);
  vout.Uv = calculate_vertex_uv(vin);

  return vout;
}


