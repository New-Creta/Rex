#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/shader_reflection/bound_resource_reflection.h"
#include "rex_renderer_core/gfx/shader_visibility.h"

namespace rex
{
	namespace gfx
	{
		struct ViewRange
		{
			ViewRange(s32 baseRegister, s32 numViews, ShaderResourceType type, s32 registerSpace)
				: base_register(baseRegister)
				, num_views(numViews)
				, type(type)
				, register_space(registerSpace)
			{}

			s32 base_register;
			s32 num_views;
			ShaderResourceType type;
			s32 register_space;
		};

		struct ViewTable
		{
			ViewTable(s32 slot, rsl::vector<ViewRange>&& ranges, ShaderVisibility shaderVis)
				: slot(slot)
				, ranges(rsl::move(ranges))
				, visibility(shaderVis)
			{}

			s32 slot;
			rsl::vector<ViewRange> ranges;
			ShaderVisibility visibility;
		};
		using ShaderParameter2 = ViewTable;
	}
}