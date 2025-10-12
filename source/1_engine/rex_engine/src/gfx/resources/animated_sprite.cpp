#include "rex_engine/gfx/resources/animated_sprite.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
	namespace gfx
	{
		void AnimatedSprite::tick(f32 dt)
		{
			if (m_active_animation->has_finished_animation(m_current_sprite_idx, m_current_sprite_frame_counter))
			{
				m_current_sprite_idx = m_active_animation->next_sprite_idx(m_current_sprite_idx);
				m_current_sprite_frame_counter = 0;
			}

			m_current_sprite_frame_counter++;
		}

		void AnimatedSprite::set_animation(const rex::FlipbookAnimation* newActiveAnimation)
		{
			REX_ASSERT_X(newActiveAnimation, "New animation is nullptr, this is not allowed");
			m_active_animation = newActiveAnimation;
		}
	}
}