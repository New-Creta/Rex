#include "rex_engine/gfx/resources/unordered_access_buffer.h"

namespace rex
{
	namespace gfx
	{
		UnorderedAccessBuffer::UnorderedAccessBuffer(u64 gpuAddress, ResourceView* view, rsl::memory_size size)
			: Buffer(size)
			, m_gpu_address(gpuAddress)
			, m_resource_view(view)
		{}

		// Return the resource view of a unordered access buffer
		const ResourceView* UnorderedAccessBuffer::resource_view() const
		{
			return m_resource_view;
		}
		// Return the gpu address of a unordered access buffer
		u64 UnorderedAccessBuffer::gpu_address() const
		{
			return m_gpu_address;
		}

	}
}