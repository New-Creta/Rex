#pragma once

#include "rex_engine/gfx/resources/depth_stencil_buffer.h"
#include "rex_engine/gfx/system/resource_view.h"

#include "rex_directx/resources/dx_resource.h"
#include "rex_directx/system/dx_view_heap.h"

namespace rex
{
	namespace gfx
	{
		class DxDepthStencilBuffer : public DepthStencilBuffer, public DxResource
		{
		public:
			DxDepthStencilBuffer(const wrl::ComPtr<ID3D12Resource>& resource, DxResourceView view, s32 width, s32 height, TextureFormat format, const ClearStateDesc& clearStateDesc);

			// Returns the resource view
			DxResourceView* view();

			void* api_object() override;

		private:
			DxResourceView m_view;
		};
	}
}