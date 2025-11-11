#pragma once

#include "rex_std/vector.h"

#include "rex_engine/gfx/system/shader_parameters_store.h"
#include "rex_engine/gfx/shader_reflection/shader_param_declaration.h"

namespace rex
{
	namespace gfx
	{
		struct ShaderPipelineReflection
		{
			// This holds the parameters that are needed for the shader pipeline, but does not hold any data for them
			// It just lists the expected type of resources that are expected to be bound and what slots they'll be bound to
			rsl::vector<ShaderParameterDeclaration> parameters;

			// This holds a lookup table from resource name to location within the above parameters
			// These lookup tables are split based on what's expected to bind them
			ShaderParametersStoreDesc material_param_store_desc;
			ShaderParametersStoreDesc renderpass_param_store_desc;
		};
	}
}