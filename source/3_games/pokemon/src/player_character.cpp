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
		if (m_walking_counter > 0)
		{
			m_animated_sprite->tick(dt);
			continue_movement();
		}
	}
	void PlayerCharacter::handle_input(const rex::InputInfo& inputInfo)
	{
		m_input_mappings->handle_input(inputInfo);
	}

	//void PlayerCharacter::init_animations()
	//{
	//	m_animations = rex::asset_db::instance()->load<rex::Flipbook>("pokemon/anims/player_anim.json");
	//}
	void PlayerCharacter::init_input()
	{
		m_input_mappings = rex::asset_db::instance()->load<rex::InputMapping>("Pokemon/inputs/player_input.json");

		m_input_mappings->bind_action("up", [this](const rex::InputInfo& info) { move_up(info); });
		m_input_mappings->bind_action("down", [this](const rex::InputInfo& info) { move_down(info); });
		m_input_mappings->bind_action("left", [this](const rex::InputInfo& info) { move_left(info); });
		m_input_mappings->bind_action("right", [this](const rex::InputInfo& info) { move_right(info); });
	}
	void PlayerCharacter::init_gfx_proxy()
	{
		rex::Flipbook* animations = rex::asset_db::instance()->load<rex::Flipbook>("pokemon/anims/player_anim.json");
		m_animated_sprite = rex::gfx::gal::instance()->create_animated_sprite(animations);
		face_down();
	}

	void PlayerCharacter::move_up(const rex::InputInfo& info)
	{
		if (m_facing_direction != rex::Direction::North)
		{
			face_up();
			m_facing_direction = rex::Direction::North;
		}
		else
		{
			walk_up();
		}
	}
	void PlayerCharacter::move_down(const rex::InputInfo& info)
	{
		if (m_facing_direction != rex::Direction::South)
		{
			face_down();
			m_facing_direction = rex::Direction::South;
		}
		else
		{
			walk_down();
		}
	}
	void PlayerCharacter::move_left(const rex::InputInfo& info)
	{
		if (m_facing_direction != rex::Direction::West)
		{
			face_left();
			m_facing_direction = rex::Direction::West;
		}
		else
		{
			walk_left();
		}
	}
	void PlayerCharacter::move_right(const rex::InputInfo& info)
	{
		if (m_facing_direction != rex::Direction::East)
		{
			face_right();
			m_facing_direction = rex::Direction::East;
		}
		else
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
		m_walking_counter = 4;
		m_animated_sprite->set_animation("up_walk");
	}
	void PlayerCharacter::walk_down()
	{
		m_walking_counter = 4;
		m_animated_sprite->set_animation("down_walk");
	}
	void PlayerCharacter::walk_left()
	{
		m_walking_counter = 4;
		m_animated_sprite->set_animation("left_walk");
	}
	void PlayerCharacter::walk_right()
	{
		m_walking_counter = 4;
		m_animated_sprite->set_animation("right_walk");
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

		m_walking_counter--;
	}

	TileCoord PlayerCharacter::pos() const
	{
		return m_pos;
	}
	void PlayerCharacter::set_pos(TileCoord pos)
	{
		m_pos = pos;
	}
}