#pragma once

#include "rex_renderer_core/resources/shader.h"

namespace rex
{
	namespace gfx
	{
		class Shader;

		// A shader pipeline object holds all the shaders will be used in a pipeline state
		struct ShaderPipeline
		{
			rsl::unique_ptr<Shader> vs; // The vertex shader
			rsl::unique_ptr<Shader> ps; // The pixel shader
		};
	}
}