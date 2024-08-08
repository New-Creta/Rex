#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/shader_reflection/bound_resource_reflection.h"
#include "rex_renderer_core/gfx/shader_visibility.h"

namespace rex
{
	namespace gfx
	{
		struct ViewRangeDeclaration
		{
			ViewRangeDeclaration(s32 baseRegister, s32 numViews, ShaderParameterType type, s32 registerSpace)
				: base_register(baseRegister)
				, num_views(numViews)
				, type(type)
				, register_space(registerSpace)
			{}

			s32 base_register;
			s32 num_views;
			ShaderParameterType type;
			s32 register_space;
		};

		struct ShaderParameterDeclaration
		{
			ShaderParameterDeclaration(s32 slot, rsl::vector<ViewRangeDeclaration>&& ranges, s32 totalNumViews, ShaderParameterType type, ShaderVisibility shaderVis)
				: slot(slot)
				, ranges(rsl::move(ranges))
				, type(type)
				, total_num_views(totalNumViews)
				, visibility(shaderVis)
			{}

			s32 slot;
			s32 total_num_views;
			ShaderParameterType type;
			rsl::vector<ViewRangeDeclaration> ranges;
			ShaderVisibility visibility;
		};
		using ShaderParameterDeclaration = ShaderParameterDeclaration;
	}
}