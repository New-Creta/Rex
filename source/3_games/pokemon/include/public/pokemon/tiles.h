#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/engine/globals.h"

#include "rex_engine/gfx/resources/upload_buffer.h"

namespace rex
{
	namespace gfx
	{
		class Texture2D;
		class UnordedAccessBuffer;
	}
}

namespace pokemon
{
	class TilesManager
	{
	public:
		TilesManager();
		s16 load(rsl::string_view tilesetPath, s16 dstStartIdx);
		void set_indices(s16 x, s16 y, rsl::initializer_list<s32> indices);

		void render();

	private:
		rsl::unique_ptr<rex::gfx::Texture2D> m_tilemap;
		rsl::unique_ptr<rex::gfx::UnorderedAccessBuffer> m_tile_indices;
	};

	namespace tiles
	{
		void init(rex::globals::GlobalUniquePtr<TilesManager> tileManager);
		TilesManager* instance();
		void shutdown();
	}
}