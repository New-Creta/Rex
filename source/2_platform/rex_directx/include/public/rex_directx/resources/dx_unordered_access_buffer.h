#pragma once

#include "rex_engine/gfx/resources/unordered_access_buffer.h"

#include "rex_directx/utility/dx_util.h"
#include "rex_directx/system/dx_resource_view.h"
#include "rex_directx/resources/dx_resource.h"

namespace rex
{
	namespace gfx
	{
		class DxUnorderedAccessBuffer : public UnorderedAccessBuffer, public DxResource
		{
		public:
			DxUnorderedAccessBuffer(const wrl::ComPtr<ID3D12Resource>& resource, DxResourceView resourceView, rsl::memory_size size);

			// Returns the resource view
			DxResourceView* view();

		private:
			DxResourceView m_resource_view;
		};

	}
}