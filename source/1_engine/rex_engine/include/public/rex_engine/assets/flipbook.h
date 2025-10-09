#pragma once

#include "rex_std/bonus/math.h"

#include "rex_engine/assets/texture_asset.h"
#include "rex_engine/assets/asset.h"

#include "rex_std/vector.h"

namespace rex
{
	class FlipbookSprite
	{
	private:
		s32 m_idx;
		s32 m_num_frames;
		bool m_flip_x;
	};

	class FlipbookAnimation
	{
	public:
		FlipbookAnimation(rsl::string_view name, rsl::vector<FlipbookSprite>&& sprites);

		rsl::string_view name() const;

	private:
		rsl::string m_name;
		rsl::vector<FlipbookSprite> m_sprites;
	};

	class Flipbook : public Asset
	{
	public:
		Flipbook(rsl::pointi8 spriteSize, TextureAsset* texture, rsl::vector<FlipbookAnimation>&& animations);

		const FlipbookAnimation* find_animation(rsl::string_view animationName) const;

	private:
		TextureAsset* m_texture; // the spriteset texture to use for the animations
		rsl::pointi8 m_sprite_size; // the size of a single tile in the texture	
		rsl::vector<FlipbookAnimation> m_animations;
	};
}