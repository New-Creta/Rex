#pragma once

#include "rex_std/memory.h"

#include "rex_engine/gfx/resources/texture_2d.h"

namespace pokemon
{
  class TileSet
  {
  public:
    TileSet(const u8* tilesetData);

    const u8* tile_pixel_row(u8 tileIdx, s8 pxRow) const;

  private:
    const u8* tile_data(u8 tileIdx) const;

  private:
    constexpr static s8 s_tileset_width_px = 128;
    constexpr static s8 s_tile_pixel_byte_size = 1;

    const u8* m_tileset_data;
  };

  class Tile
  {
  public:
    static constexpr s8 width_px()
    {
      return s_width_px;
    }
    static constexpr s8 height_px()
    {
      return s_height_px;
    }

  private:
    constexpr static s8 s_width_px = 8;
    constexpr static s8 s_height_px = 8;
  };

	//rsl::shared_ptr<TileSet> load_tileset(rsl::string_view filepath);
}