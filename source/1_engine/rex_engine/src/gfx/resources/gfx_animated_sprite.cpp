#include "rex_engine/gfx/resources/gfx_animated_sprite.h"

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/log.h"

#include "rex_engine/engine/casting.h"

#include "rex_engine/gfx/resources/texture_2d.h"

namespace rex
{
	namespace gfx
	{
		DEFINE_LOG_CATEGORY(LogAnimatedSprite);

		AnimatedSprite::AnimatedSprite(Flipbook* animations)
			: m_animations(animations)
			, m_active_animation(nullptr)
			, m_current_anim_frame_idx(0)
			, m_current_anim_frame_counter(0)
			, m_num_animation_ticks(0)
		{}

		void AnimatedSprite::tick()
		{
			if (!m_active_animation)
			{
				return;
			}

			if (m_active_animation->has_finished_animation(m_current_anim_frame_idx, m_current_anim_frame_counter))
			{
				m_current_anim_frame_idx = m_active_animation->next_frame_idx(m_current_anim_frame_idx);
				m_current_anim_frame_counter = 0;
			}

			m_current_anim_frame_counter++;
			m_num_animation_ticks++;
		}

		void AnimatedSprite::set_pos(rex::PixelCoord pos)
		{
			m_pos = pos;
		}
		rex::PixelCoord AnimatedSprite::pos() const
		{
			return m_pos;
		}

		void AnimatedSprite::set_animation(rsl::string_view name)
		{
			if (m_active_animation && m_active_animation->name() == name)
			{
				return;
			}

			REX_ASSERT_X(can_be_interrupted(), "A new animation was requested but the animation cannot be interrupted");

			m_active_animation = m_animations->find_animation(name);
			m_current_anim_frame_idx = 0;
			m_current_anim_frame_counter = 0;
			m_num_animation_ticks = 0;
			REX_ASSERT_X(m_active_animation, "New animation is nullptr, this is not allowed");
		}

		bool AnimatedSprite::can_be_interrupted() const
		{
			// If there's no active animation, of course, it can be interrupted
			if (!m_active_animation)
			{
				return true;
			}

			return m_active_animation->can_be_interrupted(m_num_animation_ticks);
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
			s32 sprite_idx_in_texture = m_active_animation->frame(m_current_anim_frame_idx).sprite_idx;
			s32 sprites_per_row = m_animations->sprites_texture()->texture_resource()->width() / m_animations->sprite_size().x;

			rsl::pointi8 sprite_coord{};
			sprite_coord.x = numeric_cast<s8>(sprite_idx_in_texture % sprites_per_row);
			sprite_coord.y = numeric_cast<s8>(sprite_idx_in_texture / sprites_per_row);

			rsl::point<f32> uv_start{};
			uv_start.x = (sprite_coord.x * m_animations->sprite_size().x) / static_cast<f32>(m_animations->sprites_texture()->texture_resource()->width());
			uv_start.y = (sprite_coord.y * m_animations->sprite_size().y) / static_cast<f32>(m_animations->sprites_texture()->texture_resource()->height());
			
			return uv_start;
		}

		const FlipbookAnimationFrame& AnimatedSprite::current_sprite() const
		{
			return m_active_animation->frame(m_current_anim_frame_idx);
		}
	}
}