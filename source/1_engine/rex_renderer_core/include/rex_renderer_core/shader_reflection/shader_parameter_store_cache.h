#pragma once

#include "rex_renderer_core/shader_reflection/shader_pipeline_reflection.h"
#include "rex_renderer_core/system/shader_parameters_store.h"

namespace rex
{
	namespace gfx
	{
		namespace shader_parameter_store_cache
		{
			rsl::unique_ptr<ShaderParametersStore> load(const ShaderPipeline& pipeline, ShaderParameterBinding binding);
		}
	}
}