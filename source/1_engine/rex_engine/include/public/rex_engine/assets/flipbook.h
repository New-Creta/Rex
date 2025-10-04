#pragma once

#include "rex_std/bonus/math.h"

#include "rex_engine/assets/texture_asset.h"
#include "rex_engine/assets/asset.h"

namespace rex
{
	class Flipbook : public Asset
	{
	public:
		Flipbook(rsl::pointi8 spriteSize, TextureAsset* texture);

	private:
		TextureAsset* m_texture;
		rsl::pointi8 m_sprite_size; // the size of a single tile in the texture
	};
}