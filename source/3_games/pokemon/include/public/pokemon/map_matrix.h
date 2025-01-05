#pragma once

#include "rex_engine/engine/types.h"

#include "rex_std/memory.h"

#include "pokemon/map_coordinates.h"

namespace pokemon
{
  struct MapData;
  class BlockSet;

  // The map matrix is a matrix of the map, including the padding blocks
  // Each cell in the matrix holds a block index.
  // All blocks in the map matrix are expected to use the same blockset
  class MapMatrix
  {
  public:
    MapMatrix(const MapData* mapObject);

    s32 width_in_blocks() const;
    s32 height_in_blocks() const;
    s32 major_dimension_in_blocks() const;
    s32 width_in_tiles() const;
    s32 height_in_tiles() const;
    s32 major_dimension_in_tiles() const;

    // Returns the block index at the location
    s32 index_at(BlockCoord blockCoord) const;

    // Returns the block index of the block the tile belongs to
    s32 index_at(TileCoord tileCoord) const;

  private:
    void init_blocks(s8 borderBlockIdx);
    void fill_connections(const MapData* mapObject);
    void fill_inner_map(const MapData* mapObject);

  private:
    rsl::unique_array<u8> m_block_indices;
    s8 m_total_width;
    s8 m_total_height;
  };
}