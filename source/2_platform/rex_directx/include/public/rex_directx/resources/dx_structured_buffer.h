#pragma once

#include "rex_engine/gfx/resources/structured_buffer.h"
#include "rex_directx/resources/dx_resource.h"
#include "rex_directx/system/dx_resource_view.h"

namespace rex
{
	namespace gfx
	{
		class DxStructuredBuffer : public StructuredBuffer, public DxResource
		{
		public:
			DxStructuredBuffer(wrl::ComPtr<ID3D12Resource> resource, DxResourceView view, rsl::memory_size stride, s32 numElements);

			void* api_object() const override;

		private:
			DxResourceView m_view;
		};
	}
}