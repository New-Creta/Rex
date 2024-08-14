#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/gfx/texture_format.h"
#include "rex_renderer_core/resources/resource.h"

namespace rex
{
	namespace gfx
	{
		class ResourceView;

		class DepthStencilBuffer : public Resource
		{
		public:
			DepthStencilBuffer(ResourceView* view, s32 width, s32 height, TextureFormat format);

			s32 width() const;
			s32 height() const;
			TextureFormat format() const;
			ResourceView* resource_view();

		private:
			s32 m_width;
			s32 m_height;
			TextureFormat m_format;
			ResourceView* m_view;
		};
	}
}