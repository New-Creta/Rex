#pragma once

#include "rex_engine/gfx/resources/texture_2d.h"

#include "rex_std/memory.h"

namespace regina
{
	class Thumbnail
	{
	public:
		Thumbnail(rsl::unique_ptr<rex::gfx::Texture2D> texture);

		const rex::gfx::Texture2D* texture() const;

	private:
		rsl::unique_ptr<rex::gfx::Texture2D> m_texture;
	};
}