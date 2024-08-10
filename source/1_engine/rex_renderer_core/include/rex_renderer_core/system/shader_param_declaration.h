#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/shader_reflection/bound_resource_reflection.h"
#include "rex_renderer_core/gfx/shader_visibility.h"

namespace rex
{
	namespace gfx
	{
		// A declaration describing a range of views
		// The following is assumed about the views in a range
		// - continious register numbers
		// - all belong to the same register space
		// - all have the same shader parameter type
		struct ViewRangeDeclaration
		{
			ViewRangeDeclaration(s32 baseRegister, s32 numViews, ShaderParameterType type, s32 registerSpace)
				: base_register(baseRegister)
				, num_views(numViews)
				, type(type)
				, register_space(registerSpace)
			{}

			s32 base_register;			// the register of the first view in the range
			s32 num_views;				// the number of views within the range
			ShaderParameterType type;	// the type of the views within the range
			s32 register_space;			// the space of the views within the range
		};

		// A shader parameter is a single parameter that'll be bound to a shader
		// It holds 1 or more ranges of views and each of these ranges hold 1 or more views
		// The following is assumed about every view in every range
		// - all belong to the same register space
		// - all have the same shader parameter type
		struct ShaderParameterDeclaration
		{
			ShaderParameterDeclaration(s32 slot, rsl::vector<ViewRangeDeclaration>&& ranges, s32 totalNumViews, ShaderParameterType type, ShaderVisibility shaderVis)
				: slot(slot)
				, ranges(rsl::move(ranges))
				, type(type)
				, total_num_views(totalNumViews)
				, visibility(shaderVis)
			{}

			s32 slot;									// the slot the parameter belongs to
			s32 total_num_views;						// the total number of views within the parameter, this is the sum of all the views in every range
			ShaderParameterType type;					// the type of the views within the ranges
			rsl::vector<ViewRangeDeclaration> ranges;	// the ranges the parameter holds
			ShaderVisibility visibility;				// which shader has visibility to this parameter
		};
	}
}