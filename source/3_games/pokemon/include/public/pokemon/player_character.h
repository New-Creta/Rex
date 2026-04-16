#pragma once

#include "pokemon/map_coordinates.h"

#include "rex_engine/assets/map.h"
#include "rex_engine/assets/flipbook.h"
#include "rex_engine/assets/input_mapping.h"
#include "rex_engine/engine/types.h"

#include "rex_engine/assets/animated_sprite.h"

#include "rex_std/unordered_map.h"

namespace pokemon
{
	enum class PlayerMovementState
	{
		Idle,
		Walking,
		Biking,
		Surfing,
	};

	class PlayerCharacter
	{
	public:
		PlayerCharacter();

		void tick();

		rex::PixelCoord pos() const;
		void set_pos(rex::PixelCoord pos);

	private:
		void init_input();
		void init_sprite();

		// movement
		// The below are requests but can fail if the animation
		// cannot be interrupted
		void move_up();
		void move_down();
		void move_left();
		void move_right();
		void go_idle();

		void continue_movement();
		void finish_movement();

		void move(rex::Direction direction, rsl::string_view idle_anim, rsl::string_view walk_anim);

		bool will_collide();
		void play_collision_sound();

	private:
		rex::PixelCoord m_pos;
		rex::Direction m_facing_direction;
		rex::InputMapping* m_input_mappings;
		rex::AnimatedSprite* m_animated_sprite;
		s8 m_speed;
		PlayerMovementState m_movement_state;
	};
}