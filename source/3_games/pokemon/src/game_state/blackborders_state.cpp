#include "pokemon/game_state/blackborders_state.h"

#include "rex_engine/memory/memory_types.h"
#include "rex_engine/filesystem/path.h"
#include "rex_std/bonus/utility.h"

#include "pokemon/system/tiles.h"

namespace pokemon
{
	BlackBordersState::BlackBordersState()
		: StateTask(
			[this]() { begin(); },
			[this](s32 frameIdx) { return tick(frameIdx); },
			[this]() { end(); }
		)
	{

	}

	void BlackBordersState::begin()
	{
		// gengar tilemap holds the black tile which is required for the black border rendering
		rex::scratch_string gengar_tiles = rex::path::join(rex::engine::instance()->project_root(), "gfx", "intro", "gengar.png");

		rex::scratch_string nidorino_tiles_1 = rex::path::join(rex::engine::instance()->project_root(), "gfx", "intro", "red_nidorino_1.png");
		rex::scratch_string nidorino_tiles_2 = rex::path::join(rex::engine::instance()->project_root(), "gfx", "intro", "red_nidorino_2.png");
		rex::scratch_string nidorino_tiles_3 = rex::path::join(rex::engine::instance()->project_root(), "gfx", "intro", "red_nidorino_3.png");

		s16 dst_start = 0;
		dst_start += tiles::instance()->load(gengar_tiles, dst_start);
		dst_start += tiles::instance()->load(nidorino_tiles_1, dst_start);
		dst_start += tiles::instance()->load(nidorino_tiles_2, dst_start);
		dst_start += tiles::instance()->load(nidorino_tiles_3, dst_start);
	}
	GameState BlackBordersState::tick(s32 frameIdx)
	{
		const s16 num_frames_to_be_active = 64;
		if (frameIdx > num_frames_to_be_active)
		{
			return pokemon::GameState::GamefreakLogo;
		}
		return pokemon::GameState::BlackBorders;
	}
	void BlackBordersState::end()
	{

	}
}