#pragma once

#include "rex_renderer_core/resources/shader.h"

namespace rex
{
	namespace gfx
	{
		struct ShaderPipeline
		{
			rsl::unique_ptr<Shader> vs;
			rsl::unique_ptr<Shader> ps;
		};
	}
}