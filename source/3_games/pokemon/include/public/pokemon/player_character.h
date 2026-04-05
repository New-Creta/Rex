#pragma once

#include "pokemon/map_coordinates.h"

#include "rex_engine/assets/map.h"
#include "rex_engine/assets/flipbook.h"
#include "rex_engine/assets/input_mapping.h"
#include "rex_engine/engine/types.h"

#include "rex_engine/gfx/resources/animated_sprite.h"

#include "rex_std/unordered_map.h"

namespace pokemon
{
	class PlayerCharacter
	{
	public:
		PlayerCharacter();

		void tick();

		rex::PixelCoord pos() const;
		void set_pos(rex::PixelCoord pos);

	private:
		void init_input();
		void init_gfx_proxy();

		// movement
		void move_up();
		void move_down();
		void move_left();
		void move_right();

		void continue_movement();
		void finish_movement();

		void move(rex::Direction direction, rsl::string_view idle_anim, rsl::string_view walk_anim);

	private:
		rex::PixelCoord m_pos;
		rex::InputMapping* m_input_mappings;
		rex::Direction m_facing_direction;

		rex::gfx::AnimatedSprite* m_animated_sprite;
	};
}