#include "rex_directx/resources/dx_depth_stencil_buffer.h"

namespace rex
{
	namespace gfx
	{
		DxDepthStencilBuffer::DxDepthStencilBuffer(const wrl::ComPtr<ID3D12Resource>& resource, DxResourceView view, s32 width, s32 height, TextureFormat format, const ClearStateDesc& clearStateDesc)
			: DepthStencilBuffer(&m_view, width, height, format, clearStateDesc)
			, DxResource(resource)
			, m_view(view)
		{

		}

		DxResourceView* DxDepthStencilBuffer::view()
		{
			return &m_view;
		}

		void* DxDepthStencilBuffer::api_object() const
		{
			return dx_object();
		}

	}
}