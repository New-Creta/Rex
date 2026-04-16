#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/gfx/resources/resource.h"

namespace rex
{
	namespace gfx
	{
		class ResourceView;

		class RenderTargetBase : public Resource
		{
		public:
			RenderTargetBase(s32 width, s32 height);

			s32 width() const;
			s32 height() const;

			virtual const ResourceView* view() const = 0;
			virtual const rsl::Color4f& clear_color() const = 0;

		private:
			s32 m_width;
			s32 m_height;
		};
	}
}