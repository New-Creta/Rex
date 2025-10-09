#include "rex_engine/assets/flipbook.h"

namespace rex
{
	FlipbookAnimation::FlipbookAnimation(rsl::string_view name, rsl::vector<FlipbookSprite>&& sprites)
		: m_name(name)
		, m_sprites(rsl::move(sprites))
	{}

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
}