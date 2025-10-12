#include "rex_engine/gfx/resources/animated_sprite.h"

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
			m_active_animation = newActiveAnimation;
		}
	}
}