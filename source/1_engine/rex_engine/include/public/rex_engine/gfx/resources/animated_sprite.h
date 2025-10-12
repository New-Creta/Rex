#pragma once

#include "rex_engine/assets/flipbook.h"

#include "rex_engine/engine/types.h"

namespace rex
{
	namespace gfx
	{
		class AnimatedSprite
		{
		public:
			void tick(f32 dt);

			void set_animation(const rex::FlipbookAnimation* newActiveAnimation);

		private:
			const rex::FlipbookAnimation* m_active_animation;
			s32 m_current_sprite_idx;
			s32 m_current_sprite_frame_counter;
		};
	}
}