#include "rex_directx/resources/dx_sampler_2d.h"

namespace rex
{
	namespace gfx
	{
		DxSampler2D::DxSampler2D(const DxResourceView& resourceView)
			: m_resource_view(resourceView)
		{}

		DxResourceView* DxSampler2D::gpu_handle()
		{
			return &m_resource_view;
		}
	}
}