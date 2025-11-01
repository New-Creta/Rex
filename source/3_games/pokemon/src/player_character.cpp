#include "pokemon/player_character.h"

#include "rex_engine/engine/asset_db.h"

#include "rex_engine/gfx/graphics.h"

namespace pokemon
{
	DEFINE_LOG_CATEGORY(LogPlayerCharacter);

	PlayerCharacter::PlayerCharacter()
		: m_facing_direction(rex::Direction::South)
		, m_walking_counter(0)
	{
		//init_animations();
		init_input();
		init_gfx_proxy();
	}

	void PlayerCharacter::tick(f32 dt)
	{
		//REX_INFO(LogPlayerCharacter, "new frame");

		if (m_was_moving)
		{
			m_was_moving = false;
		}

		if (m_walking_counter > 0)
		{
			m_was_moving = true;
			m_animated_sprite->tick(dt);
			continue_movement();
		}
		else
		{
			finish_movement();
		}
	}
	void PlayerCharacter::handle_input(const rex::KeyState& inputInfo)
	{
		// We've got a problem here as the engine will wait 15 frames to send a new "key down message"
		// as Windows only sends one every 15 frames
		m_input_mappings->handle_input(inputInfo);
	}

	//void PlayerCharacter::init_animations()
	//{
	//	m_animations = rex::asset_db::instance()->load<rex::Flipbook>("pokemon/anims/player_anim.json");
	//}
	void PlayerCharacter::init_input()
	{
		m_input_mappings = rex::asset_db::instance()->load<rex::InputMapping>("Pokemon/inputs/player_input.json");

		m_input_mappings->bind_action("up", [this](const rex::KeyState& info) { move_up(info); });
		m_input_mappings->bind_action("down", [this](const rex::KeyState& info) { move_down(info); });
		m_input_mappings->bind_action("left", [this](const rex::KeyState& info) { move_left(info); });
		m_input_mappings->bind_action("right", [this](const rex::KeyState& info) { move_right(info); });
	}
	void PlayerCharacter::init_gfx_proxy()
	{
		rex::Flipbook* animations = rex::asset_db::instance()->load<rex::Flipbook>("pokemon/anims/player_anim.json");
		m_animated_sprite = rex::gfx::gal::instance()->create_animated_sprite(animations);
		face_down();
	}

	void PlayerCharacter::move_up(const rex::KeyState& info)
	{
		if (m_was_moving && m_facing_direction != rex::Direction::North)
		{
			return;
		}

		if (m_facing_direction != rex::Direction::North)
		{
			face_up();
			m_facing_direction = rex::Direction::North;
		}
		else if (m_walking_counter == 0)
		{
			walk_up();
		}
	}
	void PlayerCharacter::move_down(const rex::KeyState& info)
	{
		if (m_was_moving && m_facing_direction != rex::Direction::South)
		{
			return;
		}

		if (m_facing_direction != rex::Direction::South)
		{
			face_down();
			m_facing_direction = rex::Direction::South;
		}
		else if (m_walking_counter == 0)
		{
			walk_down();
		}
	}
	void PlayerCharacter::move_left(const rex::KeyState& info)
	{
		if (m_was_moving && m_facing_direction != rex::Direction::West)
		{
			return;
		}

		if (m_facing_direction != rex::Direction::West)
		{
			face_left();
			m_facing_direction = rex::Direction::West;
		}
		else if (m_walking_counter == 0)
		{
			walk_left();
		}
	}
	void PlayerCharacter::move_right(const rex::KeyState& info)
	{
		if (m_was_moving && m_facing_direction != rex::Direction::East)
		{
			return;
		}

		if (m_facing_direction != rex::Direction::East)
		{
			face_right();
			m_facing_direction = rex::Direction::East;
		}
		else if (m_walking_counter == 0)
		{
			walk_right();
		}
	}

	void PlayerCharacter::face_up()
	{
		m_animated_sprite->set_animation("up_idle");
	}
	void PlayerCharacter::face_down()
	{
		m_animated_sprite->set_animation("down_idle");
	}
	void PlayerCharacter::face_left()
	{
		m_animated_sprite->set_animation("left_idle");
	}
	void PlayerCharacter::face_right()
	{
		m_animated_sprite->set_animation("right_idle");
	}

	void PlayerCharacter::walk_up()
	{
		m_walking_counter = 8;
		if (!m_was_moving/* && m_facing_direction != rex::Direction::North*/)
		{
			m_animated_sprite->set_animation("up_walk");
		}
	}
	void PlayerCharacter::walk_down()
	{
		m_walking_counter = 8;
		if (!m_was_moving/* && m_facing_direction != rex::Direction::South*/)
		{
			m_animated_sprite->set_animation("down_walk");
		}
	}
	void PlayerCharacter::walk_left()
	{
		m_walking_counter = 8;
		if (!m_was_moving/* && m_facing_direction != rex::Direction::West*/)
		{
			m_animated_sprite->set_animation("left_walk");
		}
	}
	void PlayerCharacter::walk_right()
	{
		m_walking_counter = 8;
		if (!m_was_moving/* && m_facing_direction != rex::Direction::East*/)
		{
			m_animated_sprite->set_animation("right_walk");
		}
	}

	void PlayerCharacter::continue_movement()
	{
		switch (m_facing_direction)
		{
		case rex::Direction::North: m_pos.y -= 2;	break;
		case rex::Direction::East:  m_pos.x += 2;	break;
		case rex::Direction::South: m_pos.y += 2;	break;
		case rex::Direction::West:  m_pos.x -= 2; break;
		}

		m_walking_counter--;
	}
	void PlayerCharacter::finish_movement()
	{
		switch (m_facing_direction)
		{
		case rex::Direction::North: face_up();	break;
		case rex::Direction::East:  face_right();	break;
		case rex::Direction::South: face_down();	break;
		case rex::Direction::West:  face_left(); break;
		}
	}

	rex::PixelCoord PlayerCharacter::pos() const
	{
		return m_pos;
	}
	void PlayerCharacter::set_pos(rex::PixelCoord pos)
	{
		m_pos = pos;
	}
}