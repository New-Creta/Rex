#pragma once

#include "rex_directx/resources/dx_resource_interface.h"

namespace rex
{
	namespace gfx
	{
		class DxResource : public DxResourceInterface
		{
		public:
			DxResource(const wrl::ComPtr<ID3D12Resource>& resource)
				: m_resource(resource)
			{}

			void debug_set_name(rsl::string_view name)
			{
				d3d::set_debug_name_for(m_resource.Get(), name);
			}

			// This is made virtual so derived classes can be made
			// that wrap resources and don't store them directly
			ID3D12Resource* dx_object() const override
			{
				return m_resource.Get();
			}

		private:
			wrl::ComPtr<ID3D12Resource> m_resource;
		};
	}
}