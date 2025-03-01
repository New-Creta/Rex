#include "regina/thumbnail.h"

namespace regina
{
	Thumbnail::Thumbnail(rsl::unique_ptr<rex::gfx::Texture2D> texture)
		: m_texture(rsl::move(texture))
	{}

	const rex::gfx::Texture2D* Thumbnail::texture() const
	{
		return m_texture.get();
	}

}