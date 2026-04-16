#include "pokemon/player_character.h"

#include "rex_engine/engine/asset_db.h"

#include "rex_engine/gfx/graphics.h"

namespace pokemon
{
	DEFINE_LOG_CATEGORY(LogPlayerCharacter);

	namespace
	{
		rsl::string_view UpIdleAnim			= "up_idle";
		rsl::string_view DownIdleAnim		= "down_idle";
		rsl::string_view LeftIdleAnim		= "left_idle";
		rsl::string_view RightIdleAnim	= "right_idle";

		rsl::string_view UpWalkAnim			= "up_walk";
		rsl::string_view DownWalkAnim		= "down_walk";
		rsl::string_view LeftWalkAnim		= "left_walk";
		rsl::string_view RightWalkAnim	= "right_walk";

	}

	PlayerCharacter::PlayerCharacter()
		: m_input_mappings(nullptr)
		, m_animated_sprite(nullptr)
		, m_pos()
		, m_facing_direction(rex::Direction::South)
		, m_speed(1)
		, m_movement_state(PlayerMovementState::Idle)
	{
		init_input();
		init_sprite();
	}

	void PlayerCharacter::tick()
	{	
		rsl::scopeguard tick_anim = [this]() { m_animated_sprite->tick(m_pos); };

		// the position and facing direction is determined by the character, but animation and collision can influence it

		// If we cannot interrupt the animation, that means we have to let it finish playing
		// However that does not mean the position needs to be updated
		// as it's possible the player is walking into a wall
		if (!m_animated_sprite->can_be_interrupted())
		{
			if (will_collide())
			{
				play_collision_sound();
			}
			else
			{
				continue_movement();
			}

			return;
		}

		// If the animation is interruptable and we're currently in an go_idle state,
		// meaning the player doesn't need to move
		// we fallback to our go_idle animations
		if (m_movement_state == PlayerMovementState::Idle)
		{
			finish_movement();
			return;
		}
	}
	void PlayerCharacter::init_input()
	{
		m_input_mappings = rex::asset_db::instance()->load<rex::InputMapping>("Pokemon/inputs/player_input.json");

		m_input_mappings->bind_action("walk_up",			[this](const rex::KeyState& info) { move_up();		});
		m_input_mappings->bind_action("walk_down",		[this](const rex::KeyState& info) { move_down();	});
		m_input_mappings->bind_action("walk_left",		[this](const rex::KeyState& info) { move_left();	});
		m_input_mappings->bind_action("walk_right",		[this](const rex::KeyState& info) { move_right(); });

		m_input_mappings->bind_action("idle_up",			[this](const rex::KeyState& info) { go_idle(); });
		m_input_mappings->bind_action("idle_down",		[this](const rex::KeyState& info) { go_idle(); });
		m_input_mappings->bind_action("idle_left",		[this](const rex::KeyState& info) { go_idle(); });
		m_input_mappings->bind_action("idle_right",		[this](const rex::KeyState& info) { go_idle();  });
	}
	void PlayerCharacter::init_sprite()
	{
		m_animated_sprite = rex::asset_db::instance()->load<rex::AnimatedSprite>("pokemon/anim_states/player.json");
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
			m_movement_state = PlayerMovementState::Idle;
			return;
		}

		// If we're already facing the same direction, and we're already walking
		// there's nothing for us to do and we can just continue with the current animation
		if (m_movement_state == PlayerMovementState::Walking)
		{
			return;
		}

		// If we are already facing the desired direction but we're not moving yet
		// start moving now
		m_movement_state = PlayerMovementState::Walking;
		m_animated_sprite->set_animation(walkAnim);
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
	void PlayerCharacter::go_idle()
	{
		// Set the movement state but don't update the animation
		// This is because the animation might need to finish before idling is allowed
		m_movement_state = PlayerMovementState::Idle;
	}

	void PlayerCharacter::continue_movement()
	{
		switch (m_facing_direction)
		{
		case rex::Direction::North: m_pos.y -= m_speed;	break;
		case rex::Direction::East:  m_pos.x += m_speed;	break;
		case rex::Direction::South: m_pos.y += m_speed;	break;
		case rex::Direction::West:  m_pos.x -= m_speed; break;
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
	}

	bool PlayerCharacter::will_collide()
	{
		return m_movement_state != PlayerMovementState::Idle && false;
	}
	
	void PlayerCharacter::play_collision_sound()
	{
		REX_STATIC_TODO("Implement collision sound");
	}

}