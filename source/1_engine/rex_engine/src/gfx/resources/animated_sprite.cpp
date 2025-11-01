#include "rex_engine/gfx/resources/animated_sprite.h"

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/log.h"

#include "rex_engine/gfx/resources/texture_2d.h"

namespace rex
{
	namespace gfx
	{
		DEFINE_LOG_CATEGORY(LogAnimatedSprite);

		AnimatedSprite::AnimatedSprite(Flipbook* animations)
			: m_animations(animations)
			, m_current_anim_sprite_idx(0)
			, m_active_animation(nullptr)
		{}

		void AnimatedSprite::tick(f32 dt)
		{
			if (!m_active_animation)
			{
				return;
			}

			if (m_active_animation->has_finished_animation(m_current_anim_sprite_idx, m_current_sprite_frame_counter))
			{
				m_current_anim_sprite_idx = m_active_animation->next_sprite_idx(m_current_anim_sprite_idx);
				m_current_sprite_frame_counter = 0;
			}

			m_current_sprite_frame_counter++;
		}

		void AnimatedSprite::set_animation(rsl::string_view name)
		{
			if (m_active_animation && m_active_animation->name() == name)
			{
				return;
			}

			m_active_animation = m_animations->find_animation(name);
			m_current_anim_sprite_idx = 0;
			m_current_sprite_frame_counter = 0;
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

		rsl::point<f32> AnimatedSprite::current_sprite_uv() const
		{
			s32 sprite_idx_in_texture = m_active_animation->sprite(m_current_anim_sprite_idx).sprite_idx;
			s32 sprites_per_row = m_animations->sprites_texture()->texture_resource()->width() / m_animations->sprite_size().x;

			rsl::pointi8 sprite_coord{};
			sprite_coord.x = sprite_idx_in_texture % sprites_per_row;
			sprite_coord.y = sprite_idx_in_texture / sprites_per_row;

			rsl::point<f32> uv_start{};
			uv_start.x = (sprite_coord.x * m_animations->sprite_size().x) / static_cast<f32>(m_animations->sprites_texture()->texture_resource()->width());
			uv_start.y = (sprite_coord.y * m_animations->sprite_size().y) / static_cast<f32>(m_animations->sprites_texture()->texture_resource()->height());
			
			return uv_start;
		}

		const FlipbookSprite& AnimatedSprite::current_sprite() const
		{
			return m_active_animation->sprite(m_current_anim_sprite_idx);
		}
	}
}