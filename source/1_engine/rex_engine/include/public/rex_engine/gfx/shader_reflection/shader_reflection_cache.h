#pragma once

#include "rex_engine/gfx/system/shader_pipeline.h"
#include "rex_std/string.h"

#include "rex_engine/gfx/system/shader_param_declaration.h"
#include "rex_engine/gfx/shader_reflection/shader_signature.h"

#include "rex_engine/gfx/system/shader_parameter_location.h"

#include "rex_engine/gfx/system/shader_parameters_store.h"

namespace rex
{
	namespace gfx
	{
		struct ShaderPipelineReflection
		{
			// This holds the parameters that are needed for the shader pipeline, but does not hold any data for them
			// It just lists the expectedtype of resources that are expected to be bound and what slots they'll be bound to
			rsl::vector<ShaderParameterDeclaration> parameters;

			// This holds a lookup table from resource name to location within the above parameters
			// These lookup tables are split based on what's expected to bind them
			ShaderParametersStoreDesc material_param_store_desc;
			ShaderParametersStoreDesc renderpass_param_store_desc;
		};

		namespace shader_reflection_cache
		{
			// Load reflection from the cache, create new one if one isn't cached already
			const ShaderPipelineReflection& load(const ShaderPipeline& pipeline);
		}
	}
}