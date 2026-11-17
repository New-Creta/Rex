#include "defines.hlsl"
#include "utils.hlsl" // for is_bit_set

// This holds the tile data stored as 2bpp (bits per pixel)
// Meaning a tile's color can only be 1 of 4 values
// 0b00 -> black
// 0b01 -> dark grey
// 0b10 -> light grey
// 0b11 -> white
// 4 pixels are packed together into a single byte
// 0xFF -> 0b11111111
// the lower 2 bits are the first pixel
// the next 2 bits are the second pixel
// and so on
ByteAddressBuffer TilesData : register(t0, RENDER_PASS_REGISTER_SPACE);

// This buffers holds the indices of each tile in the texture.
// Using this information, we can calculate the UV for each vertex
// Each index is only 1 byte, so you need to use bit shifting to unpack it
// TLDR: this maps instance id --> tile id in the texture
ByteAddressBuffer TileIndexIntoTextureBuffer : register(t1, RENDER_PASS_REGISTER_SPACE);

// Every constant buffer needs to be 256 byte aligned
// So we try to pack as much data together into 1 buffer
cbuffer RenderingMetaData : register(b0, RENDER_PASS_REGISTER_SPACE)
{
  uint screen_tiles_per_row;    // the number of tiles per tile row on the screen
  uint tilemap_tiles_per_row;    // the number of tiles per tile row in the texture

  float2 inv_tile_screen_size;  // the inverse of the width of a single tile on the screen
  float2 inv_pixel_screen_size; // the inverse size of a single vintage pixel
  
  float2 tile_tilemap_size; // the width and height of a single tile in the tilemap
  float2 inv_tile_tilemap_size; // the inverse width and height of a single tile in the tilemap
  
  int2 screen_pixel_offset; // the extra offset of a tile using vintage pixel size
};

// Vertices expected for this shader are meant to spawn the entire screen
// as in, without any transforms, it will cover { 0, 0 } until { 1, 1 } of the window
// This shader transforms the vertex positions and UVs to only span the size a single
// tile takes up in the render target and in the tileset texture

struct VertexIn
{
  float2 PosL : POSITION; // The position of the vertex in local space
  float2 Uv : TEXCOORD0; // The UV of the vertex

  uint instanceId : SV_InstanceID; // The instance the vertex belongs to
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

  // Calculate where on the screen the tile needs to go
  // The result holds 2D coordinate as indices
  // eg (x: 1, y: 3) -> tile on the 4th row, at 2nd column
  uint2 screen_tile_coord_idx = index_to_coord(vin.instanceId, screen_tiles_per_row);

  // Calculate the position of this cell, starting from top left
  float2 pos = { -1, 1 }; // start from the top left
  pos.x += vin.PosL.x * inv_tile_screen_size.x; // scale down to position to its size relative to the render target
  pos.x += (screen_tile_coord_idx.x * inv_tile_screen_size.x); // offset the vertex based on the current instance we're rendering
  pos.y += vin.PosL.y * inv_tile_screen_size.y;
  pos.y -= (screen_tile_coord_idx.y * inv_tile_screen_size.y);

  pos.x += screen_pixel_offset.x * inv_pixel_screen_size.x;
  pos.y += screen_pixel_offset.y * inv_pixel_screen_size.y;
   
  // Offset the position to this position
  return float4(pos, 0.5f, 1.0f);
}

float2 calculate_vertex_uv(VertexIn vin)
{
  // For example:
  // Let's say you have the following TileIndexIntoTextureBuffer
  // +-------+-------+-------+-------+-------+-------+-------+-------+
  // |   10  |   10  |  11   |   11  |   5   |   5   |   5   |   5   |
  // +-------+-------+-------+-------+-------+-------+-------+-------+
  // and you're currently processing a tile instance of idx 1
  // That'd mean you're expecting a final result of tile idx == 10
  // As you're processing a tile which is the 10th tile in the texture

  // The offset value from which we load needs to be 4 byte aligned.
  // see: https://learn.microsoft.com/en-us/windows/win32/direct3d11/direct3d-11-advanced-stages-cs-resources#byte-address-buffer
  // That's why we load 4 bytes from the lower aligned value based on the instance ID
  // Then we shift it so we get correct byte out and that's what we use as index
  uint tile_idx_offset = vin.instanceId & ~3; // mask away the lower bits, so we're always aligning down to a multiple of 4
  uint result = TileIndexIntoTextureBuffer.Load(tile_idx_offset); // This always loads 4 bytes, not 1 byte
  
  // Using the above example, we'd expect to have the following
  // tile_idx_offset == 0 (as 1 & ~3 == 0)
  // result == { 10, 10, 11, 11 } == 0x0A0A0B0B <-- array of 4 bytes, 1 index per byte

  uint byte_offset = vin.instanceId & 3; // limit to the value { 0, 1, 2, 3 }
  uint bit_offset = byte_offset * 8;

  // Using the above example, we'd expect to have the following
  // byte_offset == 1 (as 1 & 3 == 1)
  // bit_offset == 1 * 8 == 8

  // Now shift the byte we care about to the front and mask it off
  uint tile_idx = result >> bit_offset;
  tile_idx &= 0x000000FF;

  // Using the above example, we'd expect the following
  // tile_idx == 0x0A0B0B (after shift)
  // tile_idx == 0x0A == 10 (after mask)
  
  // We know that the tile instance we're currently processing maps to the 10th tile in the texture
  // which is exactly what we're expecting
  // We now have to move our UV coordinate to the correct location so that also maps to the UV location of the 10th tile

  // Calculate the 2D coordinate of the tile in the texture
  // The result holds 2D coordinate as indices
  // eg (x: 1, y: 3) -> tile on the 4th row, at 2nd column
  uint2 tex_tile_coord_idx = index_to_coord(tile_idx, tilemap_tiles_per_row);
  
  // Calculate the the top left uv coordinate of the tile we want
  float2 tile_uv_start;
  tile_uv_start.x = tex_tile_coord_idx.x * inv_tile_tilemap_size.x;
  tile_uv_start.y = tex_tile_coord_idx.y * inv_tile_tilemap_size.y;

  // Add the offset to the original uv offset
  float2 uv = { 0.0, 0.0 };
  uv.x += vin.Uv.x * inv_tile_tilemap_size.x;
  uv.x += tile_uv_start.x;
  uv.y += vin.Uv.y * inv_tile_tilemap_size.y;
  uv.y += tile_uv_start.y;
  
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

float4 PSMain(PS_INPUT pin) : SV_Target
{
  // UV -> integer pixel coordinate in the tileset
  uint2 px = (uint2)(pin.Uv * float2(tile_tilemap_size.x, tile_tilemap_size.y));
  px = min(px, uint2(tile_tilemap_size.x - 1, tile_tilemap_size.y - 1));
  
  // CPU packs the image row-major, 4 px per byte, 2 bits per pixel, low bits first
  uint pixel_idx = px.y * tile_tilemap_size.x + px.x;
  
  // 16 pixels per 4 bytes, address must be 4 byte aligned
  uint packed = TilesData.Load((pixel_idx / 16) * 4);
  uint value = (packed >> ((pixel_idx % 16) * 2)) & 0x3;
  
  float c = value / 3.0f; // 0 = black ... 3 = white
  return float4(c, c, c, 1.0f);
  
  
  
  
  
  
  
  // As the tilemap texture is a byte address buffer
  // storing pixesl as 2bpp, we cannot use traditional sampling of a texture
  // and must therefore sample it ourselves by unpacking the bits
  
  
  //uint num_bits_per_byte = 8;
  //uint num_bits_per_pixel = 2;
  //uint num_pixels_per_tile = 8;
  //uint num_tiles_per_row = 16;
  //uint num_tiles_per_column = 24;
  
  //// The size in bytes of a single row of pixels of a single tile
  //uint single_tile_px_row_byte_size = num_pixels_per_tile * num_bits_per_pixel / num_bits_per_byte;
  //// The size in bytes of an entire row of pixels of a single row in the tilemap "texture"
  //uint tile_px_row_byte_size = single_tile_px_row_byte_size * num_tiles_per_row;
  //// The size in bytes of an entire row of tiles in the tilemap "texture"
  //uint tile_row_byte_size = tile_px_row_byte_size * num_pixels_per_tile;
  
  //// Figuring out the pixel offset is a bit tricky, but we can do this using the UV
  //float single_tile_uv_size_x = 1.0f / num_tiles_per_row;
  //float single_tile_uv_size_y = 1.0f / num_tiles_per_column;
  
  //uint tile_row_idx = pin.Uv.y / single_tile_uv_size_y;
  //uint tile_column_idx = pin.Uv.x / single_tile_uv_size_x;
  //uint tile_offset = (tile_row_idx * tile_row_byte_size) + (tile_column_idx * single_tile_px_row_byte_size);
  
  //// Tile offset points to the beginning of the first byte of the tile's data.
  //// We now have to offset this to point to the byte holding the data of the pixels we're after
  //uint num_pixels_per_row = num_pixels_per_tile * num_tiles_per_row;
  //uint num_pixels_per_column = num_pixels_per_tile * num_tiles_per_column;

  //uint pixel_on_x = pin.Uv.x / num_pixels_per_row % 8;
  //uint pixel_on_y = pin.Uv.y / num_pixels_per_column % 8;
  



  
  //uint aligned_tile_px_offset = tile_offset & ~3;
  //uint result = TilesData.Load(aligned_tile_px_offset); // This always loads 4 bytes, not 1 byte
  
  //uint byte_offset = tile_offset & 1; // limit to the value { 0, 1 }
  //uint bit_offset = byte_offset * 8 * single_tile_px_row_byte_size;

  //// Now shift the byte we care about to the front and mask it off
  //uint packed_tile_color_value = result >> bit_offset;
  //packed_tile_color_value &= 0x0000FFFF;
  
  //// Get the color value out of the packed 16-bit color value
  //float single_pixel_uv_size_x = 1.0f / (num_tiles_per_row * num_pixels_per_tile);
  //float single_pixel_uv_size_y = 1.0f / (num_tiles_per_column * num_pixels_per_tile);
  
  //uint num_pixel_x = (pin.Uv.x / single_pixel_uv_size_x) % num_pixels_per_tile;
  //uint num_pixel_y = (pin.Uv.y / single_pixel_uv_size_y) % num_pixels_per_tile;
  
  //uint color_value = packed_tile_color_value >> (num_pixel_x * num_bits_per_pixel);
  //color_value &= 3;
  //color_value /= 3.0f;
  
  //float3 color = float3(color_value, color_value, color_value);
  //return float4(color, 1.0f);
}


