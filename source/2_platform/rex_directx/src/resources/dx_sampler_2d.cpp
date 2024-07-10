#include "rex_directx/resources/dx_sampler_2d.h"

namespace rex
{
	namespace gfx
	{
		DxResourceView* DxSampler2D::gpu_handle()
		{
			return &m_resource_view;
		}
	}
}