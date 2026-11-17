#include "rex_engine/assets/texture_asset.h"

#include "rex_engine/gfx/resources/texture_2d.h"

#include "rex_engine/gfx/graphics.h"
#include "rex_engine/gfx/resources/texture_2d.h"

namespace rex
{
	TextureAsset::TextureAsset(u8* data, s32 width, s32 height, gfx::TextureFormat textureFormat)
	{
		m_texture_resource = gfx::gal::instance()->create_texture2d(width, height, textureFormat, data);
	}
	TextureAsset::TextureAsset(rsl::Rgba* data, s32 width, s32 height, gfx::TextureFormat textureFormat)
	{
		m_texture_resource = gfx::gal::instance()->create_texture2d(width, height, textureFormat, data);
	}

	TextureAsset::~TextureAsset() = default;

	s32 TextureAsset::width() const
	{
		return m_texture_resource->width();
	}
	s32 TextureAsset::height() const
	{
		return m_texture_resource->height();
	}

	const gfx::Texture2D* TextureAsset::texture_resource() const
	{
		return m_texture_resource.get();
	}

}