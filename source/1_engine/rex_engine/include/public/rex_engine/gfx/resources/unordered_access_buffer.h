#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/gfx/resources/buffer.h"

#include "rex_std/bonus/memory.h"

namespace rex
{
	namespace gfx
	{
		class ResourceView;

		// A class representing a buffer shaders have random access to
		// As in, the shader determines at which offset it wants to read from this buffer
		class UnorderedAccessBuffer : public Buffer
		{
		public:
			UnorderedAccessBuffer(u64 gpuAddress, ResourceView* view, rsl::memory_size size);

			// Return the resource view of a unordered access buffer
			const ResourceView* resource_view() const;
			// Return the gpu address of a unordered access buffer
			u64 gpu_address() const;

		private:
			ResourceView* m_resource_view;
			u64 m_gpu_address;
		};
	}
}