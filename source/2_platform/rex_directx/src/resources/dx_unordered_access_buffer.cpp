#include "rex_directx/resources/dx_unordered_access_buffer.h"

namespace rex
{
	namespace gfx
	{
		DxUnorderedAccessBuffer::DxUnorderedAccessBuffer(const wrl::ComPtr<ID3D12Resource>& resource, DxResourceView resourceView, rsl::memory_size size)
			: UnorderedAccessBuffer(resource->GetGPUVirtualAddress(), &m_resource_view, size)
			, m_resource(resource)
			, m_resource_view(resourceView)
		{}

		ID3D12Resource* DxUnorderedAccessBuffer::dx_object()
		{
			return m_resource.Get();
		}

		DxResourceView* DxUnorderedAccessBuffer::view()
		{
			return &m_resource_view;
		}

	}
}