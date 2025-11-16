#pragma once

#include "rex_std/bonus/math.h"

#include "rex_engine/assets/texture_asset.h"
#include "rex_engine/assets/asset.h"

#include "rex_std/vector.h"

namespace rex
{
	struct FlipbookAnimationFrame
	{
		s32 sprite_idx;		// the sprite index within the texture to use
		s32 num_frames;		// the number of frames how long this frame should be visibile for the animation
		bool flip_x;			// flip x values, left becomes right and vice versa
		bool flip_y;			// flip x values, top becomes bottom and vice versa
	};

	// A flipbook animation holds a set of frames it should display
	// each frame holds information about itself, for example, how long it should be displayed
	class FlipbookAnimation
	{
	public:
		FlipbookAnimation(rsl::string_view name, rsl::vector<FlipbookAnimationFrame>&& sprites);

		// Given a sprite index and the number of frames its displayed on screen
		// return if it has reached its limit already or not
		bool has_finished_animation(s32 spriteIdx, s32 frameIdx) const;

		s32 next_frame_idx(s32 currentFrameIdx) const;

		const FlipbookAnimationFrame& frame(s32 idx) const;
		rsl::string_view name() const;

	private:
		rsl::string m_name;
		rsl::vector<FlipbookAnimationFrame> m_frames;
	};

	// A flipbook is a structure holding a set of animations
	// where each animation is a sequence of sprites
	class Flipbook : public Asset
	{
	public:
		Flipbook(rsl::pointi8 spriteSize, TextureAsset* texture, rsl::vector<FlipbookAnimation>&& animations);

		const FlipbookAnimation* find_animation(rsl::string_view animationName) const;

		const TextureAsset* sprites_texture() const;
		rsl::pointi8 sprite_size() const;

	private:
		TextureAsset* m_texture;											// the spriteset texture to use for the animations
		rsl::pointi8 m_sprite_size;										// the size of a single tile in the texture	
		rsl::vector<FlipbookAnimation> m_animations;  
	};
}