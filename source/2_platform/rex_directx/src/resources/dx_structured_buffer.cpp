#include "rex_directx/resources/dx_structured_buffer.h"

namespace rex
{
	namespace gfx
	{
		DxStructuredBuffer::DxStructuredBuffer(wrl::ComPtr<ID3D12Resource> resource, DxResourceView view, rsl::memory_size stride, s32 numElements)
			: StructuredBuffer(resource->GetGPUVirtualAddress(), &m_view, stride, numElements)
			, DxResource(resource)
			, m_view(view)
		{

		}

		void* DxStructuredBuffer::api_object()
		{
			return dx_object();
		}
	}
}