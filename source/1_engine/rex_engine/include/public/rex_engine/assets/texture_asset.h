#pragma once

#include "rex_engine/assets/asset.h"

#include "rex_engine/engine/types.h"

#include "rex_engine/gfx/core/texture_format.h"

#include "rex_std/memory.h"

namespace rex
{
	namespace gfx
	{
		class Texture2D;
	}

	class TextureAsset : public Asset
	{
	public:
		TextureAsset(u8* data, s32 width, s32 height, gfx::TextureFormat textureFormat);
		TextureAsset(rsl::Rgba* data, s32 width, s32 height, gfx::TextureFormat textureFormat);
		~TextureAsset();

		s32 width() const;
		s32 height() const;

		const gfx::Texture2D* texture_resource() const;

	private:
		// The texture resource for the renderer
		rsl::unique_ptr<gfx::Texture2D> m_texture_resource;
	};
}