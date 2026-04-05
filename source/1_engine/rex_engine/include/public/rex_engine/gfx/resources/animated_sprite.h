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
		// An animated sprite is a sprite on the screen
		// that is backboned by animations (other than a static sprite image)
		// Meaning that this sprite object can update the sprite it displays at runtime
		// Which sprite is active is determined by the game
		// All this object is responsible for is making sure the sprit's data
		// is send to the gpu so it can be rendered
		class AnimatedSprite
		{
		public:
			AnimatedSprite(Flipbook* animations);

			// If there's an animation active
			// tick this animation
			void tick();

			// Needed for rendering
			void set_pos(PixelCoord pos);
			PixelCoord pos() const;

			void set_animation(rsl::string_view name);

			// This is essentially a wrapped on the call towards the current active anim
			bool can_be_interrupted() const;

			// Used for rendering
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

			// The total number of frames done in the active animation
			s32 m_num_animation_ticks;

			// The tick counter for the current active frame
			s32 m_current_anim_frame_counter;

			REX_STATIC_WARNING("Ideally this should not need a position. The position should be determined by the object that it's rendering")
			rex::PixelCoord m_pos;
		};
	}
}