#include "rex_directx/resources/dx_depth_stencil_buffer.h"

namespace rex
{
	namespace gfx
	{
		DxDepthStencilBuffer::DxDepthStencilBuffer(const wrl::ComPtr<ID3D12Resource>& resource, DxResourceView view, s32 width, s32 height, TextureFormat format, const ClearStateDesc& clearStateDesc)
			: DepthStencilBuffer(&m_view, width, height, format, clearStateDesc)
			, m_resource(resource)
			, m_view(view)
		{

		}

		ID3D12Resource* DxDepthStencilBuffer::dx_object()
		{
			return m_resource.Get();
		}
		DxResourceView* DxDepthStencilBuffer::view()
		{
			return &m_view;
		}
	}
}