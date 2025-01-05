#pragma once

#include "rex_engine/gfx/rendering/renderer.h"
#include "pokemon/tileset.h"
#include "pokemon/blockset.h"

namespace pokemon
{
	class MapRenderer : public rex::gfx::Renderer
	{
	public:
		void new_frame() override;
		void render() override;

		void submit_block(const Block& block);

	private:
		rsl::unique_ptr<TileSet> m_active_tileset;
		rsl::unique_ptr<BlockSet> m_active_blockset;

	};
}