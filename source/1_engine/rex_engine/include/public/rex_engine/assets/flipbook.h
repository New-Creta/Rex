#pragma once

#include "rex_std/bonus/math.h"

#include "rex_engine/assets/texture_asset.h"
#include "rex_engine/assets/asset.h"

#include "rex_std/vector.h"

namespace rex
{
	struct FlipbookSprite
	{
		s32 sprite_idx;
		s32 num_frames;
		bool flip_x;
	};

	class FlipbookAnimation
	{
	public:
		FlipbookAnimation(rsl::string_view name, rsl::vector<FlipbookSprite>&& sprites);

		bool has_finished_animation(s32 spriteIdx, s32 frameIdx) const;
		s32 next_sprite_idx(s32 currentSpriteIdx) const;

		const FlipbookSprite& sprite(s32 idx) const;
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