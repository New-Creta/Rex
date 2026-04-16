#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/gfx/shader_reflection/shader_resource_declaration.h"
#include "rex_engine/gfx/shader_reflection/shader_param_declaration.h"

namespace rex
{
	namespace gfx
	{
		// Build a view table, which is a 2D array of resources
		// a view table is used for all resources except constant buffers
		class ViewTableBuilder
		{
		public:
			ViewTableBuilder(s32 registerSpace, ShaderParameterType expectedResourceType);

			s32 add_resource(const ShaderResourceDeclaration& resource);

			ShaderParameterDeclaration build(s32 slot, ShaderVisibility visibility);

		private:
			void add_new_range();

		private:
			s32 m_start_register;														// The start register of a new range being added to the table
			s32 m_expected_register;												// the expected register of a new resource to be aded to the table
			s32 m_expected_register_space;									// the expected register space of a new resource to be added to the table
			s32 m_total_num_views;													// the total number of views within the table, an accumulation of the number of views of every range within the table
			ShaderParameterType m_expected_resource_type;		// the expected resource type to be added to the table. all ranges should have the same resource type
			rsl::vector<ViewRangeDeclaration> m_ranges;			// all the ranges within the table
		};
	}
}