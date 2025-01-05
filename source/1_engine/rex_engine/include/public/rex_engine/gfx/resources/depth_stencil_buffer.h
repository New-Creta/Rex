#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/gfx/core/texture_format.h"
#include "rex_engine/gfx/resources/resource.h"
#include "rex_engine/gfx/resources/clear_state.h"

namespace rex
{
	namespace gfx
	{
		class ResourceView;

		// A depth stencil buffer is a buffer holding depth and/or stencil data
		// Main purpose of this is to be used for depth testing
		// But it can also be used for stencil testing, which
		// is a more advanced form of rendering
		class DepthStencilBuffer : public Resource
		{
		public:
			DepthStencilBuffer(ResourceView* view, s32 width, s32 height, TextureFormat format, const ClearStateDesc& clearStateDesc);

			// Return the width of the depth stencil buffer
			s32 width() const;
			// Return the height of the depth stencil buffer
			s32 height() const;
			// Return the format of the depth stencil buffer
			TextureFormat format() const;
			// Return the resource view of the depth stencil buffer
			ResourceView* resource_view();
			// Return the clear state of the depth stencil buffer
			const ClearStateDesc& clear_state() const;

		private:
			s32 m_width;
			s32 m_height;
			TextureFormat m_format;
			ResourceView* m_view;
			ClearStateDesc m_clear_state;
		};
	}
}