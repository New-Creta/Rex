#include "pokemon/game_state/gamefreak_logo_state.h"

#include "rex_engine/memory/memory_types.h"
#include "rex_engine/filesystem/path.h"

#include "pokemon/tiles.h"
#include "pokemon/oam.h"

#include "pokemon/render_constants.h"

namespace pokemon
{
	GamefreakLogoState::GamefreakLogoState()
		: StateTask(
			[this]() { begin(); },
			[this](s32 frameIdx) { return tick(frameIdx); },
			[this]() { end(); }
		)
	{

	}

	void GamefreakLogoState::begin()
	{
		rex::scratch_string gamefreak_text = rex::path::join(rex::engine::instance()->project_root(), "gfx", "splash", "gamefreak_presents.png");
		rex::scratch_string gamefreak_logo = rex::path::join(rex::engine::instance()->project_root(), "gfx", "splash", "gamefreak_logo.png");
		rex::scratch_string shooting_star = rex::path::join(rex::engine::instance()->project_root(), "gfx", "battle", "move_anim_1.png");
		rex::scratch_string falling_star = rex::path::join(rex::engine::instance()->project_root(), "gfx", "splash", "falling_star.png");

		s16 num_tiles_loaded = tiles::instance()->load(gamefreak_text, 0x600);
		tiles::instance()->load(gamefreak_logo, 0x600 + num_tiles_loaded);
		tiles::instance()->load(shooting_star, 0x20, 0x03, 1); // copy the third tile in the list, which is the top left quadrant of the star
		tiles::instance()->load(shooting_star, 0x21, 0x13, 1); // copy the 13th tile in the list, which is the bottom left quadarant of the star
		tiles::instance()->load(falling_star, 0x22);

		s8 x_size = constants::g_tile_width_px;
		s8 y_size = constants::g_tile_height_px;

		// GameFreakLogoOAMData
		// setting it from the 24th object until the 39th
		oam::instance()->set(24, OAMStruct{ rex::narrow_cast<s8>(x_size * 10), rex::narrow_cast<s8>(y_size * 9),  0x8di8, OAMAttributes::None});
		oam::instance()->set(25, OAMStruct{ rex::narrow_cast<s8>(x_size * 11), rex::narrow_cast<s8>(y_size * 9),  0x8ei8, OAMAttributes::None });
		oam::instance()->set(26, OAMStruct{ rex::narrow_cast<s8>(x_size * 10), rex::narrow_cast<s8>(y_size * 10), 0x8fi8, OAMAttributes::None });
		oam::instance()->set(27, OAMStruct{ rex::narrow_cast<s8>(x_size * 11), rex::narrow_cast<s8>(y_size * 10), 0x90i8, OAMAttributes::None });
		oam::instance()->set(28, OAMStruct{ rex::narrow_cast<s8>(x_size * 10), rex::narrow_cast<s8>(y_size * 11), 0x91i8, OAMAttributes::None });
		oam::instance()->set(29, OAMStruct{ rex::narrow_cast<s8>(x_size * 11), rex::narrow_cast<s8>(y_size * 11), 0x92i8, OAMAttributes::None });
		oam::instance()->set(30, OAMStruct{ rex::narrow_cast<s8>(x_size * 6),  rex::narrow_cast<s8>(y_size * 12), 0x80i8, OAMAttributes::None });
		oam::instance()->set(31, OAMStruct{ rex::narrow_cast<s8>(x_size * 7),  rex::narrow_cast<s8>(y_size * 12), 0x81i8, OAMAttributes::None });
		oam::instance()->set(32, OAMStruct{ rex::narrow_cast<s8>(x_size * 8),  rex::narrow_cast<s8>(y_size * 12), 0x82i8, OAMAttributes::None });
		oam::instance()->set(33, OAMStruct{ rex::narrow_cast<s8>(x_size * 9),  rex::narrow_cast<s8>(y_size * 12), 0x83i8, OAMAttributes::None });
		oam::instance()->set(34, OAMStruct{ rex::narrow_cast<s8>(x_size * 10), rex::narrow_cast<s8>(y_size * 12), 0x93i8, OAMAttributes::None });
		oam::instance()->set(35, OAMStruct{ rex::narrow_cast<s8>(x_size * 11), rex::narrow_cast<s8>(y_size * 12), 0x84i8, OAMAttributes::None });
		oam::instance()->set(36, OAMStruct{ rex::narrow_cast<s8>(x_size * 12), rex::narrow_cast<s8>(y_size * 12), 0x85i8, OAMAttributes::None });
		oam::instance()->set(37, OAMStruct{ rex::narrow_cast<s8>(x_size * 13), rex::narrow_cast<s8>(y_size * 12), 0x83i8, OAMAttributes::None });
		oam::instance()->set(38, OAMStruct{ rex::narrow_cast<s8>(x_size * 14), rex::narrow_cast<s8>(y_size * 12), 0x81i8, OAMAttributes::None });
		oam::instance()->set(39, OAMStruct{ rex::narrow_cast<s8>(x_size * 15), rex::narrow_cast<s8>(y_size * 12), 0x86i8, OAMAttributes::None });

		// shooting start OAM data
		oam::instance()->set(0, OAMStruct{ rex::narrow_cast<s8>(x_size * 20), 0, 0xa0i8, OAMAttributes::DmgPalette });
		oam::instance()->set(1, OAMStruct{ rex::narrow_cast<s8>(x_size * 20), 0, 0xa0i8, rsl::add_flag(OAMAttributes::DmgPalette, OAMAttributes::FlipX) });
		oam::instance()->set(2, OAMStruct{ rex::narrow_cast<s8>(x_size * 20), 0, 0xa1i8, OAMAttributes::DmgPalette });
		oam::instance()->set(3, OAMStruct{ rex::narrow_cast<s8>(x_size * 20), 0, 0xa1i8, rsl::add_flag(OAMAttributes::DmgPalette, OAMAttributes::FlipX) });

	}
	GameLoopState GamefreakLogoState::tick(s32 frameIdx)
	{
		// Load this when the big star loop has finished
		load_small_start_oam();
	}
	void GamefreakLogoState::end()
	{

	}

	void GamefreakLogoState::load_small_start_oam()
	{
		// SmallStarsOAM
		OAMStruct small_stars_oam = OAMStruct{ 0, 0, 0xa2i8, rsl::add_flag(OAMAttributes::BehindBg, OAMAttributes::DmgPalette) };
		for (s32 i = 0; i < 24; ++i)
		{
			oam::instance()->set(i, small_stars_oam);
		}
	}

}