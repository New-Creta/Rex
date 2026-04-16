#pragma once

#include "rex_engine/engine/types.h"


namespace rex
{
	namespace gfx
	{
		// A shader parameter can be stored at a certain location in a parameter store
		// This object describes where to find a view for a certain resource that's bound to a shader
		struct ShaderParameterLocation
		{
			s32 slot;					// The parameter slot where the resource is stored
			s32 idx;					// The index within the shader parameters vector in a store that parameter can be found
			s32 view_offset;	// The offset within a list of views where the resource can be found. This is only relevant for "ViewTableShaderParam" parameters, who store more than a single view
		};
	}
}