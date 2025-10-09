#pragma once

#include "pokemon/map_coordinates.h"

#include "rex_engine/assets/map.h"
#include "rex_engine/assets/flipbook.h"
#include "rex_engine/assets/input_mapping.h"
#include "rex_engine/engine/types.h"

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

	private:
		void init_animations();
		void init_input();

		void move_up(const rex::InputInfo& info);
		void move_down(const rex::InputInfo& info);
		void move_left(const rex::InputInfo& info);
		void move_right(const rex::InputInfo& info);

	private:
		TileCoord m_pos;
		rex::InputMapping* m_input_mappings;
		rex::Flipbook* m_animations;
		const rex::FlipbookAnimation* m_active_animation;
		rex::Direction m_facing_direction;
	};
}