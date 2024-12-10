#include "defines.hlsl"

// This buffers holds the indices of each tile in the texture.
// Using this information, we can calculate the UV for each vertex
// Each index is only 1 byte, so you need to use bit shifting to unpack it
ByteAddressBuffer TileIndexBuffer : register(t0, RENDER_PASS_REGISTER_SPACE);

// Every constant buffer needs to be 256 byte aligned
// So we try to pack as much data together into 1 buffer
cbuffer RenderingMetaData : register(b0, RENDER_PASS_REGISTER_SPACE)
{
  // Tile texture data
  int texture_tiles_per_row;
  float inv_texture_width;
  float inv_texture_height;

  // Render target data
  uint screen_width_in_tiles;   // the number of tiles we render on a single row
  float inv_tile_screen_width;   // the inverse of the width of a single tile on the screen
  float inv_tile_screen_height;  // the inverse of the height of a single tile on the screen
};

struct VertexIn
{
  float2 PosL : POSITION;       // The position of the vertex in local space
  float2 Uv : TEXCOORD0;        // The UV of the vertex

  uint instanceId : SV_InstanceID;  // The instance the vertex belongs to
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

VertexOut main(VertexIn vin)
{
  VertexOut vout;

  // Calculate where on the screen the tile needs to go
  uint2 screen_tile_coord = index_to_coord(vin.instanceId, screen_width_in_tiles);

  // Calculate the position of this cell, starting from top left
  float2 pos = { -1, 1 };
  pos.x += screen_tile_coord.x * inv_tile_screen_width;
  pos.y -= screen_tile_coord.y * inv_tile_screen_height;

  // Offset the position to this position
  vout.PosH = float4(vin.PosL + pos, 0.0, 1.0f);

  // The offset value from which we load needs to be 4 byte aligned.
  // see: https://learn.microsoft.com/en-us/windows/win32/direct3d11/direct3d-11-advanced-stages-cs-resources#byte-address-buffer
  // That's why we load 4 bytes from the lower aligned value based on the instance ID
  // Then we shift it so we get right byte out and that's what we use as index
  uint tile_idx_offset = vin.instanceId & ~3; // mask away the lower bits, so we're always aligning down to a multiple of 4
  uint result = TileIndexBuffer.Load(tile_idx_offset); // This always loads 4 bytes, not 1 byte
  
  uint byte_offset = vin.instanceId & 3; // limit to the value { 0, 1, 2, 3 }
  uint bit_offset = byte_offset * 8;

  // Now shift the byte we care about to the front and mask it off
  uint tile_idx = result >> bit_offset;
  tile_idx &= 0x000000FF;

  // Calculate the 2D coordinate of the tile in the texture
  uint2 tex_tile_coord = index_to_coord(tile_idx, texture_tiles_per_row);
  
  int tile_width_px = 8;
  int tile_height_px = 8;

  // Calculate the the top left uv coordinate of the tile we want
  float2 tile_uv_start;
  tile_uv_start.x = tile_width_px * tex_tile_coord.x * inv_texture_width;
  tile_uv_start.y = tile_height_px * tex_tile_coord.y * inv_texture_height;

  // Add it to the original uv offset
  vout.Uv = vin.Uv + tile_uv_start;

  return vout;
}


