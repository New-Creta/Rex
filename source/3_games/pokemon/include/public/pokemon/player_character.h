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

		void tick(f32 dt);
		void handle_input(const rex::InputInfo& inputInfo);

		TileCoord pos() const;
		void set_pos(TileCoord pos);

	private:
		//void init_animations();
		void init_input();
		void init_gfx_proxy();

		// movement
		void move_up(const rex::InputInfo& info);
		void move_down(const rex::InputInfo& info);
		void move_left(const rex::InputInfo& info);
		void move_right(const rex::InputInfo& info);

		void face_up();
		void face_down();
		void face_left();
		void face_right();

		void walk_up();
		void walk_down();
		void walk_left();
		void walk_right();

		void continue_movement();

	private:
		TileCoord m_pos;
		rex::InputMapping* m_input_mappings;
		//rex::Flipbook* m_animations;
		rex::Direction m_facing_direction;

		rex::gfx::AnimatedSprite* m_animated_sprite;

		s32 m_walking_counter;
	};
}