#include "rex_engine/gfx/resources/animated_sprite.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
	namespace gfx
	{
		AnimatedSprite::AnimatedSprite(Flipbook* animations)
			: m_animations(animations)
		{}

		void AnimatedSprite::tick(f32 dt)
		{
			if (!m_active_animation)
			{
				return;
			}

			if (m_active_animation->has_finished_animation(m_current_sprite_idx, m_current_sprite_frame_counter))
			{
				m_current_sprite_idx = m_active_animation->next_sprite_idx(m_current_sprite_idx);
				m_current_sprite_frame_counter = 0;
			}

			m_current_sprite_frame_counter++;
		}

		void AnimatedSprite::set_animation(rsl::string_view name)
		{
			m_active_animation = m_animations->find_animation(name);
			REX_ASSERT_X(m_active_animation, "New animation is nullptr, this is not allowed");
		}

		const Texture2D* AnimatedSprite::sprites_texture() const
		{
			return m_animations->sprites_texture()->texture_resource();
		}
		rsl::pointi8 AnimatedSprite::sprite_size() const
		{
			return m_animations->sprite_size();
		}

	}
}