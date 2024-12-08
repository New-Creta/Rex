#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/gfx/resources/texture_2d.h"
#include "rex_engine/gfx/resources/vertex_buffer.h"
#include "rex_engine/gfx/resources/index_buffer.h"
#include "rex_engine/gfx//resources/sampler_2d.h"
#include "rex_engine/gfx/system/rhi.h"
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
  struct TileRendererDesc
  {
    s32 width_in_tiles;
    s32 height_in_tiles;
    const rex::gfx::Texture2D* tileset_texture;
    const BlockSet* blockset;
  };
  struct TileVertex
  {
    rsl::point<f32> pos;
    rsl::point<f32> uv;
  };
  struct TileReadonlyInstanceData
  {
    glm::mat4 wvp;
  };
  struct TileMutableInstanceData
  {
    s32 tile_idx;
  };
  struct TextureData
  {
    s32 tiles_per_row;
    f32 inv_texture_width;
    f32 inv_texture_height;
  };

  class TileRenderer : public rex::gfx::Renderer
  {
  public:
    TileRenderer(const TileRendererDesc& desc);

    void new_frame() override;
    void render() override;

    void update_tileset_texture(const rex::gfx::Texture2D* texture);
    void update_dimension(s32 width, s32 height);

    void update_tile_data(const MapMatrix& mapMatrix, TileCoord playerPos);

  private:
    s32 expected_num_vertices() const;
    s32 expected_num_tiles() const;
    s32 expected_num_wvps() const;
    s32 expected_num_indices() const;

    void init();
    void init_vb();
    void init_instance_vb();
    void init_ib();
    void init_cb();
    void init_renderpass();

    void upload_vertex_buffer();

  private:
    s32 m_width; // width in tiles
    s32 m_height; // height in tiles
    const rex::gfx::Texture2D* m_tileset_texture; // the tileset texture to use
    const BlockSet* m_blockset; // The blockset to use to retrieve the tileset indices from

    // Tiles vertex buffers and index buffer
    rsl::unique_ptr<rex::gfx::VertexBuffer> m_tiles_vb_gpu; // This holds the 4 vertices used to make up a plane
    rsl::unique_ptr<rex::gfx::VertexBuffer> m_tiles_instances_immutable_vb_gpu; // This holds the matrices for the tiles. This never changes at runtime
    rsl::unique_ptr<rex::gfx::VertexBuffer> m_tiles_instances_vb_gpu; // This holds the tile indices. The UV coordinates are created from this
    rsl::unique_ptr<rex::gfx::IndexBuffer> m_tiles_ib_gpu; // This holds the indices for every tile. This never changes at runtime
    rsl::unique_ptr<rex::gfx::ConstantBuffer> m_constant_buffer;

    // The tile cache is like a map matrix but it holds indices on a tile level instead of block level
    rsl::unique_array<s32> m_tile_cache;

    // The render pass which holds all the information needed to render a single frame
    rsl::unique_ptr<rex::gfx::RenderPass> m_render_pass;
    rsl::unique_ptr<rex::gfx::Sampler2D> m_fonts_sampler;          // The sampler for the fonts texture
    rex::gfx::Viewport m_viewport;
    rex::gfx::ScissorRect m_scissor_rect;
  };
}