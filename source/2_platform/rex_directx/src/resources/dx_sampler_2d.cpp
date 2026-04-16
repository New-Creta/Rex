#include "rex_directx/resources/dx_sampler_2d.h"

namespace rex
{
	namespace gfx
	{
		DxSampler2D::DxSampler2D(const DxResourceView& resourceView)
			: Sampler2D(&m_resource_view)
			, m_resource_view(resourceView)
		{}

		// Return a view to the object on the gpu
		DxResourceView* DxSampler2D::view()
		{
			return &m_resource_view;
		}

		void* DxSampler2D::api_object() const
		{
			// DirectX doesn't have a resource object for samplers
			// It's all done through resource views
			return nullptr;
		}
	}
}