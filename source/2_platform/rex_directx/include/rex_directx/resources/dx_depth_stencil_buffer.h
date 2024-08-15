#pragma once

#include "rex_renderer_core/resources/depth_stencil_buffer.h"
#include "rex_renderer_core/system/resource_view.h"

#include "rex_directx/system/dx_view_heap.h"

namespace rex
{
	namespace gfx
	{
		class DxDepthStencilBuffer : public DepthStencilBuffer
		{
		public:
			DxDepthStencilBuffer(const wrl::ComPtr<ID3D12Resource>& resource, DxResourceView view, s32 width, s32 height, TextureFormat format, const ClearStateDesc& clearStateDesc);

			// Return the wrapped Direct X resource
			ID3D12Resource* dx_object();
			// Returns the resource view
			DxResourceView* view();

		private:
			wrl::ComPtr<ID3D12Resource> m_resource;
			DxResourceView m_view;
		};
	}
}