#pragma once

#include "rex_engine/assets/asset.h"
#include "rex_engine/assets/flipbook.h"
#include "rex_engine/math/coords.h"

#include "glm/glm.hpp"

namespace rex
{
	namespace gfx
	{
		struct SpriteRenderProxy;
	}

	class AnimatedSprite : public Asset
	{
	public:
		AnimatedSprite(Flipbook* animations);

		void tick(rex::PixelCoord spritePos);
		void set_animation(rsl::string_view name);

		bool can_be_interrupted() const;
		
	private:
		void update_render_proxy(rex::PixelCoord spritePos);
		glm::vec2 current_sprite_uv() const;
		const FlipbookAnimationFrame* active_animation_frame() const;

	private:
		Flipbook* m_animations;
		const FlipbookAnimation* m_active_animation;

		// The frame within the animation that is active
		s32 m_current_anim_frame_idx;

		// The total number of frames done in the active animation
		s32 m_num_animation_ticks;

		// The tick counter for the current active frame
		s32 m_current_anim_frame_counter;

		gfx::SpriteRenderProxy* m_render_proxy;
	};
}