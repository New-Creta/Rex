#pragma once

#include "rex_engine/gfx/resources/sampler_2d.h"

#include "rex_directx/system/dx_resource_view.h"

namespace rex
{
	namespace gfx
	{
		class DxSampler2D : public Sampler2D
		{
		public:
			DxSampler2D(const DxResourceView& resourceView);

			// Return a view to the object on the gpu
			DxResourceView* view();

		private:
			DxResourceView m_resource_view;
		};
	}
}