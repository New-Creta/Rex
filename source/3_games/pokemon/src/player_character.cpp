#include "pokemon/player_character.h"

#include "rex_engine/engine/asset_db.h"

#include "rex_engine/gfx/graphics.h"

namespace pokemon
{
	DEFINE_LOG_CATEGORY(LogPlayerCharacter);

	namespace
	{
		rsl::string_view UpIdleAnim = "up_idle";
		rsl::string_view DownIdleAnim = "down_idle";
		rsl::string_view LeftIdleAnim = "left_idle";
		rsl::string_view RightIdleAnim = "right_idle";

		rsl::string_view UpWalkAnim = "up_walk";
		rsl::string_view DownWalkAnim = "down_walk";
		rsl::string_view LeftWalkAnim = "left_walk";
		rsl::string_view RightWalkAnim = "right_walk";

	}

	PlayerCharacter::PlayerCharacter()
		: m_facing_direction(rex::Direction::South)
	{
		init_input();
		init_gfx_proxy();
	}

	void PlayerCharacter::tick()
	{	
		if (m_animated_sprite->can_be_interrupted())
		{
			finish_movement();
		}
		else
		{
			continue_movement();
		}
		
		m_animated_sprite->tick();
		m_animated_sprite->set_pos(m_pos);
	}
	void PlayerCharacter::init_input()
	{
		m_input_mappings = rex::asset_db::instance()->load<rex::InputMapping>("Pokemon/inputs/player_input.json");

		m_input_mappings->bind_action("up", [this](const rex::KeyState& info) { REX_UNUSED_PARAM(info); move_up(); });
		m_input_mappings->bind_action("down", [this](const rex::KeyState& info) { REX_UNUSED_PARAM(info); move_down(); });
		m_input_mappings->bind_action("left", [this](const rex::KeyState& info) { REX_UNUSED_PARAM(info); move_left(); });
		m_input_mappings->bind_action("right", [this](const rex::KeyState& info) { REX_UNUSED_PARAM(info); move_right(); });
	}
	void PlayerCharacter::init_gfx_proxy()
	{
		rex::Flipbook* animations = rex::asset_db::instance()->load<rex::Flipbook>("pokemon/anims/player_anim.json");
		m_animated_sprite = rex::gfx::gal::instance()->create_animated_sprite(animations);
		m_animated_sprite->set_animation(DownIdleAnim);
	}

	void PlayerCharacter::move(rex::Direction direction, rsl::string_view idleAnim, rsl::string_view walkAnim)
	{
		// If we're moving already but not in the direction indicate by the direction for this func
		// then do not do anything as we have to finish our walking animation before we're allowed to process input
		if (!m_animated_sprite->can_be_interrupted() && m_facing_direction != direction)
		{
			return;
		}

		// If we're not facing the direction we need to, face it first but do not move
		if (m_facing_direction != direction)
		{
			m_animated_sprite->set_animation(idleAnim);
			m_facing_direction = direction;
			return;
		}

		// If we are already facing the desired direction but we're not moving yet
		// start moving now
		if (m_animated_sprite->can_be_interrupted())
		{
			m_animated_sprite->set_animation(walkAnim);
		}
	}

	void PlayerCharacter::move_up()
	{
		move(rex::Direction::North, UpIdleAnim, UpWalkAnim);
	}
	void PlayerCharacter::move_down()
	{
		move(rex::Direction::South, DownIdleAnim, DownWalkAnim);
	}
	void PlayerCharacter::move_left()
	{
		move(rex::Direction::West, LeftIdleAnim, LeftWalkAnim);
	}
	void PlayerCharacter::move_right()
	{
		move(rex::Direction::East, RightIdleAnim, RightWalkAnim);
	}

	void PlayerCharacter::continue_movement()
	{
		switch (m_facing_direction)
		{
		case rex::Direction::North: m_pos.y -= 1;	break;
		case rex::Direction::East:  m_pos.x += 1;	break;
		case rex::Direction::South: m_pos.y += 1;	break;
		case rex::Direction::West:  m_pos.x -= 1; break;
		}
	}
	void PlayerCharacter::finish_movement()
	{
		switch (m_facing_direction)
		{
		case rex::Direction::North: m_animated_sprite->set_animation(UpIdleAnim);    break;
		case rex::Direction::East:  m_animated_sprite->set_animation(RightIdleAnim); break;
		case rex::Direction::South: m_animated_sprite->set_animation(DownIdleAnim);  break;
		case rex::Direction::West:  m_animated_sprite->set_animation(LeftIdleAnim);  break;
		}
	}

	rex::PixelCoord PlayerCharacter::pos() const
	{
		return m_pos;
	}
	void PlayerCharacter::set_pos(rex::PixelCoord pos)
	{
		m_pos = pos;
		m_animated_sprite->set_pos(m_pos);
	}
}