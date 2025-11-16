#pragma once

#include "rex_engine/engine/types.h"

#include "rex_engine/gfx/resources/resource.h"
#include "rex_engine/gfx/system/resource_view.h"

#include "rex_std/bonus/memory.h"

namespace rex
{
	namespace gfx
	{
		class StructuredBuffer : public Resource
		{
		public:
			StructuredBuffer(u64 gpuAddress, ResourceView* view, rsl::memory_size stride, s32 numElements);

			u64 gpu_address() const;
			const ResourceView* view() const;
			rsl::memory_size size() const;
			rsl::memory_size stride() const;
			s32 num_elements() const;

		private:
			u64 m_gpu_address;
			ResourceView* m_view;
			rsl::memory_size m_stride;
			s32 m_num_elements;

		};
	}
}