#include "pokemon/game_state/copyright_state.h"

#include "rex_engine/memory/memory_types.h"
#include "rex_engine/engine/engine.h"
#include "rex_engine/filesystem/path.h"

#include "pokemon/tiles.h"

namespace pokemon
{
	CopyrightState::CopyrightState()
		: StateTask(
			[this]() { begin(); },
			[this](s32 frameIdx) { return tick(frameIdx); },
			[this]() { end(); }
		)
	{

	}

	void CopyrightState::begin()
	{
		// Load the copyright tiles into memory and send it to the gpu
		rex::scratch_string copyright_tiles = rex::path::join(rex::engine::instance()->project_root(), "gfx", "splash", "copyright.png");
		rex::scratch_string gamefreak_tiles = rex::path::join(rex::engine::instance()->project_root(), "gfx", "title", "gamefreak_inc.png");

		s32 dst_start = 0;
		dst_start += tiles::instance()->load(copyright_tiles, dst_start);
		dst_start += tiles::instance()->load(gamefreak_tiles, dst_start);

		tiles::instance()->set_indices(
			2, 7,
			{
				0x60, 0x61, 0x62, 0x61, 0x63, 0x61, 0x64, 0x7F, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A,										// ©'95.'96.'98 Nintendo
				0x60, 0x61, 0x62, 0x61, 0x63, 0x61, 0x64, 0x7F, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72,				// ©'95.'96.'98 	 inc.
				0x60, 0x61, 0x62, 0x61, 0x63, 0x61, 0x64, 0x7F, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, // ©'95.'96.'98 GAME FREAK inc.
			}
			);
	}
	GameLoopState CopyrightState::tick(s32 frameIdx)
	{
		const s16 num_frames_to_be_active = 180;
		if (frameIdx > num_frames_to_be_active)
		{
			return pokemon::GameLoopState::BlackBorders;
		}
		return pokemon::GameLoopState::Copyright;
	}
	void CopyrightState::end()
	{

	}
}