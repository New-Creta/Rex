#include "rex_engine/gfx/resources/structured_buffer.h"

namespace rex
{
	namespace gfx
	{
		StructuredBuffer::StructuredBuffer(u64 gpuAddress, ResourceView* view, rsl::memory_size stride, s32 numElements)
			: m_gpu_address(gpuAddress)
			, m_view(view)
			, m_stride(stride)
			, m_num_elements(numElements)
		{}

		u64 StructuredBuffer::gpu_address() const
		{
			return m_gpu_address;
		}
		const ResourceView* StructuredBuffer::view() const
		{
			return m_view;
		}
		rsl::memory_size StructuredBuffer::size() const
		{
			return m_stride.size_in_bytes() * m_num_elements;
		}
		rsl::memory_size StructuredBuffer::stride() const
		{
			return m_stride;
		}
		s32 StructuredBuffer::num_elements() const
		{
			return m_num_elements;
		}
	}
}