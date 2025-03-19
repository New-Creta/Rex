#pragma once

#include "rex_engine/gfx/resources/unordered_access_buffer.h"

#include "rex_directx/utility/dx_util.h"
#include "rex_directx/system/dx_resource_view.h"

namespace rex
{
	namespace gfx
	{
		class DxUnorderedAccessBuffer : public UnorderedAccessBuffer
		{
		public:
			DxUnorderedAccessBuffer(const wrl::ComPtr<ID3D12Resource>& resource, DxResourceView resourceView, rsl::memory_size size);

			// Return the wrapped Direct X resource
			ID3D12Resource* dx_object();
			// Returns the resource view
			DxResourceView* view();

		private:
			wrl::ComPtr<ID3D12Resource> m_resource;
			DxResourceView m_resource_view;
		};

	}
}