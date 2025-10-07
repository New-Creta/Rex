#include "rex_engine/assets/flipbook.h"

namespace rex
{
	Flipbook::Flipbook(rsl::pointi8 spriteSize, TextureAsset* texture)
		: m_sprite_size(spriteSize)
		, m_texture(texture)
	{}
}