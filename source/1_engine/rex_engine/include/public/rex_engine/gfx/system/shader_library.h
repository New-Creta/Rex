#pragma once

#include "rex_engine/gfx/resources/shader.h"
#include "rex_engine/gfx/system/shader_type.h"

namespace rex
{
	namespace gfx
	{
		namespace shader_lib
		{
			// Load a shader from the cache, given a filepath and shader type
			Shader* load(rsl::string_view path, ShaderType type);
			// Load a shader from the cache, given a shader name
			Shader* load(rsl::string_view name);
			// Clear the cache
			void clear();
		}
	}
}