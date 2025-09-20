#include "rex_directx/resources/dx_backbuffer_render_target.h"

#include "rex_directx/resources/dx_render_target.h"

namespace rex
{
	namespace gfx
	{
		ID3D12Resource* DxBackBufferRenderTarget::dx_object()
		{
			d3d::to_dx12(swapchain()->current_buffer())->dx_object();
		}
	}
}