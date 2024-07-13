#pragma once

#include "rex_renderer_core/resources/sampler_2d.h"

#include "rex_directx/system/dx_resource_view.h"

namespace rex
{
	namespace gfx
	{
		class DxSampler2D : public Sampler2D
		{
		public:
			DxSampler2D(const DxResourceView& resourceView);

			DxResourceView* gpu_handle();

		private:
			DxResourceView m_resource_view;
		};
	}
}