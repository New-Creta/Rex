#include "rex_engine/gfx/shader_reflection/view_table_builder.h"

#include "rex_engine/diagnostics/assert.h"

#include "rex_std/bonus/utility.h"

namespace rex
{
	namespace gfx
	{
		ViewTableBuilder::ViewTableBuilder(s32 registerSpace, ShaderParameterType expectedResourceType)
			: m_start_register(0)
			, m_expected_register(0)
			, m_total_num_views(0)
			, m_expected_register_space(registerSpace)
			, m_expected_resource_type(expectedResourceType)
		{}

		ViewOffset ViewTableBuilder::add_resource(const ShaderResourceDeclaration& resource)
		{
			REX_ASSERT_X(resource.register_space == m_expected_register_space, "Invalid register space for resource. expected: {} actual: {} resource: {}", m_expected_register, resource.register_space, resource.name);
			REX_ASSERT_X(resource.resource_type == m_expected_resource_type, "Invalid resource type for resource. expected: {}, actual: {}, resource: {}", rsl::enum_refl::enum_name(m_expected_resource_type), rsl::enum_refl::enum_name(resource.resource_type), resource.name);

			if (resource.shader_register != m_expected_register)
			{
				add_new_range();
				m_start_register = resource.shader_register;
			}

			m_expected_register = resource.shader_register + 1;

			ViewOffset view_offset{};
			view_offset.abs_offset = 0;
			for (const ViewRangeDeclaration& range : m_ranges)
			{
				view_offset.abs_offset += range.num_views;
			}
			view_offset.abs_offset += resource.shader_register - m_start_register;

			return view_offset;
		}

		ShaderParameterDeclaration ViewTableBuilder::build(s32 slot, ShaderVisibility visibility)
		{
			add_new_range();
			return ShaderParameterDeclaration(slot, rsl::move(m_ranges), m_total_num_views, m_expected_resource_type, visibility);
		}

		void ViewTableBuilder::add_new_range()
		{
			s32 num_views = m_expected_register - m_start_register;
			if (num_views > 0)
			{
				m_total_num_views += num_views;
				m_ranges.emplace_back(m_start_register, num_views, m_expected_resource_type, m_expected_register_space);
			}
		}
	}
}