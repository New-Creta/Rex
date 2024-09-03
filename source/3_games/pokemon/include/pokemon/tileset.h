#pragma once

#include "rex_std/memory.h"

#include "rex_engine/gfx/resources/texture_2d.h"

namespace pokemon
{
  class TileSet
  {
  public:
    TileSet(rsl::unique_ptr<rex::gfx::Texture2D> texture);

  private:
    rsl::unique_ptr<rex::gfx::Texture2D> m_texture;
  };

	rsl::shared_ptr<TileSet> load_tileset(rsl::string_view filepath);
}