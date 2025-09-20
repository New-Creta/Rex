#pragma once

#include "rex_directx/utility/dx_util.h"

namespace rex
{
	namespace gfx
	{
		class DxResourceInterface
		{
		public:
			virtual ~DxResourceInterface() = default;

			virtual void debug_set_name(rsl::string_view name) = 0;

			// This is made virtual so derived classes can be made
			// that wrap resources and don't store them directly
			virtual ID3D12Resource* dx_object() = 0;
		};
	}
}