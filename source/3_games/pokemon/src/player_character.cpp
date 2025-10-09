#include "pokemon/player_character.h"

#include "rex_engine/engine/asset_db.h"

namespace pokemon
{
	PlayerCharacter::PlayerCharacter()
	{
		init_animations();
		init_input();
	}

	void PlayerCharacter::tick(f32 dt)
	{
		//m_player_sprite_proxy->update_render_data(data);
	}
	void PlayerCharacter::handle_input(const rex::InputInfo& inputInfo)
	{
		m_input_mappings->handle_input(inputInfo);
	}

	void PlayerCharacter::init_animations()
	{
		m_animations = rex::asset_db::instance()->load<rex::Flipbook>("pokemon/sprites/player_anim.json");
	}
	void PlayerCharacter::init_input()
	{
		m_input_mappings = rex::asset_db::instance()->load<rex::InputMapping>("Pokemon/inputs/player_input.json");

		m_input_mappings->bind_action("up", [this](const rex::InputInfo& info) { move_up(info); });
		m_input_mappings->bind_action("down", [this](const rex::InputInfo& info) { move_down(info); });
		m_input_mappings->bind_action("left", [this](const rex::InputInfo& info) { move_left(info); });
		m_input_mappings->bind_action("right", [this](const rex::InputInfo& info) { move_right(info); });
	}

	void PlayerCharacter::move_up(const rex::InputInfo& info)
	{
		if (m_facing_direction == rex::Direction::North)
		{
			m_active_animation = m_animations->find_animation("up_walk");
		}
		else
		{
			m_active_animation = m_animations->find_animation("up_idle");
		}
	}
	void PlayerCharacter::move_down(const rex::InputInfo& info)
	{
		if (m_facing_direction == rex::Direction::North)
		{
			m_active_animation = m_animations->find_animation("down_walk");
		}
		else
		{
			m_active_animation = m_animations->find_animation("down_idle");
		}
	}
	void PlayerCharacter::move_left(const rex::InputInfo& info)
	{
		if (m_facing_direction == rex::Direction::North)
		{
			m_active_animation = m_animations->find_animation("left_walk");
		}
		else
		{
			m_active_animation = m_animations->find_animation("left_idle");
		}
	}
	void PlayerCharacter::move_right(const rex::InputInfo& info)
	{
		if (m_facing_direction == rex::Direction::North)
		{
			m_active_animation = m_animations->find_animation("right_walk");
		}
		else
		{
			m_active_animation = m_animations->find_animation("right_idle");
		}
	}

	TileCoord PlayerCharacter::pos() const
	{
		return m_pos;
	}
}