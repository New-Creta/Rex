#pragma once

#include "rex_engine/assets/flipbook.h"

#include "rex_engine/engine/types.h"
#include "rex_engine/math/coords.h"

namespace rex
{
	class Flipbook;
	struct FlipbookAnimationFrame;

	namespace gfx
	{
		class AnimatedSprite
		{
		public:
			AnimatedSprite(Flipbook* animations);

			// If there's an animation active
			// tick this animation
			void tick(f32 dt);

			void set_pos(rex::PixelCoord pos);
			rex::PixelCoord pos() const;


			void set_animation(rsl::string_view name);
			const Texture2D* sprites_texture() const;
			rsl::pointi8 sprite_size() const;

			// get the top left UV coordinates of the current sprite
			// of the active animation
			rsl::point<f32> current_sprite_uv() const;
			const FlipbookAnimationFrame& current_sprite() const;

		private:
			// The flipbook holding all animations for this sprite
			Flipbook* m_animations;

			// The current active animation
			const rex::FlipbookAnimation* m_active_animation;

			// The frame within the animation that is active
			s32 m_current_anim_frame_idx;

			// The tick counter for the current active frame
			s32 m_current_anim_frame_counter;
			rex::PixelCoord m_pos;
		};
	}
}