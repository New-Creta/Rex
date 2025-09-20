#pragma once

#include "rex_engine/gfx/resources/backbuffer_render_target.h"

#include "rex_directx/utility/dx_util.h"

namespace rex
{
	namespace gfx
	{
		class DxBackBufferRenderTarget : public BackBufferRenderTarget, public DxResourceInterface
		{
		public:
			ID3D12Resource* dx_object() override;

		private:

		};
	}
}