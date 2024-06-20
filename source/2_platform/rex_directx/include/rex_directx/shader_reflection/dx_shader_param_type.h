#pragma once

#include "rex_directx/utility/dx_util.h"

#include "rex_renderer_core/shader_reflection/shader_param_type.h"

namespace rex
{
	namespace gfx
	{
    DXGI_FORMAT param_type_to_format(ShaderParameterType type);
	}
}