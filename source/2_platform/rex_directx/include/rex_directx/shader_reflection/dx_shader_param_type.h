#pragma once

#include "rex_directx/utility/dx_util.h"

namespace rex
{
	namespace gfx
	{
    enum class ParameterType
    {
      Unknown,
      Uint,
      Uint2,
      Uint16_2,
      Float,
      Float2,
      Float3,
      Float4
    };

    DXGI_FORMAT param_type_to_format(ParameterType type);
	}
}