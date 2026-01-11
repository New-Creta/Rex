#include "rex_directx/resources/dx_unordered_access_buffer.h"

namespace rex
{
	namespace gfx
	{
		DxUnorderedAccessBuffer::DxUnorderedAccessBuffer(const wrl::ComPtr<ID3D12Resource>& resource, DxResourceView resourceView, rsl::memory_size size)
			: UnorderedAccessBuffer(resource->GetGPUVirtualAddress(), &m_resource_view, size)
			, DxResource(resource)
			, m_resource_view(resourceView)
		{}

		DxResourceView* DxUnorderedAccessBuffer::view()
		{
			return &m_resource_view;
		}

		void* DxUnorderedAccessBuffer::api_object() const
		{
			return dx_object();
		}

	}
}