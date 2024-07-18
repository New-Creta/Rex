#include "rex_directx/resources/dx_sampler_2d.h"

namespace rex
{
	namespace gfx
	{
		DxSampler2D::DxSampler2D(const DxResourceView& resourceView)
			: m_resource_view(resourceView)
		{}

		// Return a view to the object on the gpu
		DxResourceView* DxSampler2D::view()
		{
			return &m_resource_view;
		}
	}
}