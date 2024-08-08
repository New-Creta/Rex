#pragma once

#include "rex_renderer_core/resources/resource.h"

namespace rex
{
	namespace gfx
	{
		class ResourceView;

		// A sampler is an object that holds the settings how a texture is applied on a triangle
		class Sampler2D : public Resource
		{
		public:
			Sampler2D(ResourceView* view)
				: m_view(view)
			{}
			virtual ~Sampler2D() = default;

			ResourceView* resource_view()
			{
				return m_view;
			}

		private:
			ResourceView* m_view;
		};
	}
}