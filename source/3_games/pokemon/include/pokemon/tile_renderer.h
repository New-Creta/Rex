#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/gfx/resources/texture_2d.h"
#include "rex_engine/gfx/resources/vertex_buffer.h"
#include "rex_engine/gfx/resources/index_buffer.h"
#include "rex_engine/gfx//resources/sampler_2d.h"
#include "rex_engine/gfx//resources/unordered_access_buffer.h"
#include "rex_engine/gfx/system/gal.h"
#include "rex_engine/gfx/graphics.h"

#include "rex_engine/memory/array_view.h"
#include "rex_engine/gfx/rendering/renderer.h"
#include "rex_engine/gfx/rendering/render_pass.h"

#include "pokemon/blockset.h"
#include "pokemon/map_matrix.h"

#include "pokemon/map_coordinates.h"

#include "glm/glm.hpp"

namespace pokemon
{
 
  // Vertex used for every tile
  struct TileVertex
  {
    rsl::point<f32> pos;
    rsl::point<f32> uv;
  };

  struct TileMutableInstanceData
  {
    s8 tile_idx;
  };
  struct RenderingMetaData
  {
    // Tile texture data
    u32 texture_tiles_per_row;   // the number of tiles per row in the tileset texture
    f32 inv_texture_width;       // the inverse width of the tileset texture, in pixels
    f32 inv_texture_height;      // the inverse height of the tileset texture, in pixels

    // Render target data
    u32 screen_width_in_tiles;   // the number of tiles we render on a single row
    f32 inv_tile_screen_width;   // the inverse of the width of a single tile on the screen
    f32 inv_tile_screen_height;  // the inverse of the height of a single tile on the screen
  };

  struct TileRendererDesc
  {
    s32 width_in_tiles;
    s32 height_in_tiles;
    const rex::gfx::Texture2D* tileset_texture;
    const BlockSet* blockset;
  };

  class TileRenderer : public rex::gfx::Renderer
  {
  public:
    TileRenderer(const TileRendererDesc& desc);

    void new_frame() override;
    void render() override;

    // Update the tileset we're using to render
    void update_tileset_texture(const rex::gfx::Texture2D* texture);
    // Update the number of tiles we render on screen
    void update_dimension(s32 width, s32 height);

    // Update the tile data itself
    void update_tile_data(const MapMatrix& mapMatrix, TileCoord playerPos);

  private:
    s32 num_tiles() const;

    void init();
    void init_per_vertex_vb();
    void init_tile_indices_buffer();
    void init_ib();
    void init_cb();
    void init_renderpass();
    void init_sampler();

    void upload_tile_indices_buffer();

  private:
    s32 m_width;                                  // width in tiles
    s32 m_height;                                 // height in tiles
    const rex::gfx::Texture2D* m_tileset_texture; // the tileset texture to use
    const BlockSet* m_blockset;                   // The blockset to use to retrieve the tileset indices from

    // Tiles vertex buffers and index buffer
    rsl::unique_ptr<rex::gfx::VertexBuffer>   m_tiles_vb_gpu;                     // This holds the 4 vertices used to make up a plane
    rsl::unique_ptr<rex::gfx::IndexBuffer>    m_tiles_ib_gpu;                     // This holds the indices for every tile. This never changes at runtime
    rsl::unique_ptr<rex::gfx::ConstantBuffer> m_tile_renderer_cb;                 // This holds the extra metadata the shader needs to calculate the UVs for each instance
    rsl::unique_ptr<rex::gfx::UnorderedAccessBuffer> m_tile_indices_buffer;       // This holds the tile index within the texture for every tile on screen, starting from the top left

    // The tile cache is like a map matrix but it holds indices on a tile level instead of block level
    rsl::unique_array<s8> m_tile_cache;
    rsl::unique_array<glm::vec2> m_tile_coords;

    rsl::unique_ptr<rex::gfx::RenderPass> m_render_pass;    // The render pass which holds all the information needed to render a single frame
    rex::gfx::Sampler2D* m_default_sampler; // The sampler for the tileset texture
  };
}