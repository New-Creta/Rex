#include "rex_engine/assets/flipbook.h"

namespace rex
{
	FlipbookAnimation::FlipbookAnimation(rsl::string_view name, rsl::vector<FlipbookAnimationFrame>&& sprites)
		: m_name(name)
		, m_frames(rsl::move(sprites))
	{}

	bool FlipbookAnimation::has_finished_animation(s32 spriteIdx, s32 frameIdx) const
	{
		return frameIdx >= m_frames[spriteIdx].num_frames;
	}
	s32 FlipbookAnimation::next_frame_idx(s32 currentFrameIdx) const
	{
		return currentFrameIdx + 1 >= m_frames.size()
			? 0
			: currentFrameIdx + 1;
	}

	const FlipbookAnimationFrame& FlipbookAnimation::frame(s32 idx) const
	{
		return m_frames[idx];
	}

	rsl::string_view FlipbookAnimation::name() const
	{
		return m_name;
	}

	Flipbook::Flipbook(rsl::pointi8 spriteSize, TextureAsset* texture, rsl::vector<FlipbookAnimation>&& animations)
		: m_sprite_size(spriteSize)
		, m_texture(texture)
		, m_animations(rsl::move(animations))
	{}

	const FlipbookAnimation* Flipbook::find_animation(rsl::string_view animationName) const
	{
		auto it = rsl::find_if(m_animations.cbegin(), m_animations.cend(), [&](const FlipbookAnimation& anim)
			{
				return anim.name() == animationName;
			});

		return it != m_animations.cend()
			? rsl::iterator_to_pointer(it)
			: nullptr;
	}

	const TextureAsset* Flipbook::sprites_texture() const
	{
		return m_texture;
	}

	rsl::pointi8 Flipbook::sprite_size() const
	{
		return m_sprite_size;
	}
}