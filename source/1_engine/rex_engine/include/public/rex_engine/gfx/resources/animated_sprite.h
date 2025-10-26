#pragma once

#include "rex_engine/assets/flipbook.h"

#include "rex_engine/engine/types.h"

namespace rex
{
	class Flipbook;

	namespace gfx
	{
		class AnimatedSprite
		{
		public:
			AnimatedSprite(Flipbook* animations);

			void tick(f32 dt);

			void set_animation(rsl::string_view name);
			const Texture2D* sprites_texture() const;
			rsl::pointi8 sprite_size() const;

			rsl::point<f32> current_sprite_uv() const;
			const FlipbookSprite& current_sprite() const;

		private:
			Flipbook* m_animations;
			const rex::FlipbookAnimation* m_active_animation;
			s32 m_current_anim_sprite_idx;
			s32 m_current_sprite_frame_counter;
		};
	}
}