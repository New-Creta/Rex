#pragma once

#include "rex_renderer_core/resources/shader.h"
#include "rex_renderer_core/system/shader_type.h"

namespace rex
{
	namespace gfx
	{
		namespace shader_lib
		{
			Shader* load(rsl::string_view path, ShaderType type);
			Shader* get(rsl::string_view name);
		}
	}
}