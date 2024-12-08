#include "defines.hlsl"

cbuffer TileTextureMetaData : register(b0, RENDER_PASS_REGISTER_SPACE) 
{
  int tiles_per_row;
  float inv_width;
  float inv_height;
};

struct VertexIn
{
  float2 PosL : POSITION;             // The position of the vertex in local space
  float2 Uv : TEXCOORD0;              // The UV of the vertex

  float4x4 WVP : InstMatrix;         // The WVP matrix of the vertex
  uint TileIdx : InstTileOffset;      // The index of the tile in the texture
};

struct VertexOut
{
  float4 PosH : SV_POSITION;
  float2 Uv : TEXCOORD0;
};

int2 calc_tile_coord(uint tileIdx)
{
  uint2 coord;

  coord.x = tileIdx % tiles_per_row;
  coord.y = tileIdx / tiles_per_row;

  return coord;
}

VertexOut main(VertexIn vin)
{
  VertexOut vout;

  // Transform to homogeneous clip space.
  vout.PosH = mul(float4(vin.PosL, 0.0f, 1.0f), vin.WVP);

  // Calculate the UV from the tile index
  int2 tile_coord = calc_tile_coord(vin.TileIdx);

  int tile_width_px = 8;
  int tile_height_px = 8;

  float2 tile_uv_start;
  tile_uv_start.x = tile_width_px * tile_coord.x * inv_width;
  tile_uv_start.y = tile_height_px * tile_coord.y * inv_height;

  vout.Uv = vin.Uv + tile_uv_start;

  return vout;
}


