#pragma once

#include "rex_engine/assets/tilemap.h"
#include "rex_engine/assets/tileset_asset.h"

#include "rex_engine/engine/types.h"

#include "rex_engine/gfx/resources/render_target.h"
#include "rex_engine/gfx/resources/vertex_buffer.h"
#include "rex_engine/gfx/resources/index_buffer.h"
#include "rex_engine/gfx/resources/constant_buffer.h"
#include "rex_engine/gfx/resources/unordered_access_buffer.h"
#include "rex_engine/gfx/rendering/render_pass.h"

#include "rex_engine/gfx/system/render_context.h"

#include "rex_std/bonus/math.h"

#include "rex_std/memory.h"

namespace rex
{
	namespace gfx
	{
		class TileRenderPass
		{
		public:
			TileRenderPass(rex::gfx::RenderTarget* rt, const rex::Tilemap* tilemap, const rex::TilesetAsset* tileset);

			void set_render_target(RenderTarget* render_target);
			void set_tilemap(const rex::Tilemap* tilemap);
			void set_tileset(const rex::TilesetAsset* tileset);
			void set_tile_zoom(f32 zoom);

			void render(rex::gfx::RenderContext* renderCtx);

		private:
			void init();
			void init_vb(rex::gfx::RenderContext* renderCtx);
			void init_ib(rex::gfx::RenderContext* renderCtx);
			void init_render_info(rex::gfx::RenderContext* renderCtx);
			void init_tile_indices_uab(rex::gfx::RenderContext* renderCtx);
			void init_render_pass();

		private:
			rsl::unique_ptr<rex::gfx::VertexBuffer> m_tiles_vb_gpu;
			rsl::unique_ptr<rex::gfx::IndexBuffer> m_tiles_ib_gpu;
			rsl::unique_ptr<rex::gfx::ConstantBuffer> m_tile_render_info;
			rsl::unique_ptr<rex::gfx::UnorderedAccessBuffer> m_tiles_indices_buffer;
			rsl::unique_ptr<rex::gfx::RenderPass> m_render_pass;

			rex::gfx::RenderPassDesc m_render_pass_desc{};

			rex::gfx::RenderTarget* m_render_target;
			const rex::Tilemap* m_tilemap;
			const rex::TilesetAsset* m_tileset;

			f32 m_tile_zoom;
		};
	}
}