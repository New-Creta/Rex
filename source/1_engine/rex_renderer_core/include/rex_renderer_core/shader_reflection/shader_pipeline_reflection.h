#pragma once

#include "rex_std/vector.h"
#include "rex_engine/engine/types.h"
#include "rex_std/unordered_map.h"
#include "rex_std/bonus/string.h"
#include "rex_std/bonus/utility.h"

#include "rex_renderer_core/resources/resource.h"
#include "rex_renderer_core/system/shader_elements.h"

#include "rex_renderer_core/shader_reflection/bound_resource_reflection.h"
#include "rex_renderer_core/shader_reflection/shader_signature.h"
#include "rex_renderer_core/system/view_param.h"
#include "rex_renderer_core/system/view_table.h"

#include "rex_std/bonus/utility/output_param.h"

namespace rex
{
	namespace gfx
	{
		class ResourceView;

		enum class ShaderParameterBinding
		{
			RenderPass,
			Material
		};

		struct ShaderResourceDesc
		{
			ShaderResourceType type;
			s32 slot;
			s32 num_views;
		};

		class ShaderResource
		{
		public:
			ShaderResource(const ShaderResourceDesc& desc)
				: m_slot(desc.slot)
				, m_type(desc.type)
			{
				m_views.resize(desc.num_views);
			}

			void update_view(ViewOffset offset, ResourceView* view)
			{
				s32 abs_offset = offset.range_offset * offset.offset_within_range;
				m_views[abs_offset] = view;
			}

			const rsl::vector<ResourceView*> views() const
			{
				return m_views;
			}
			s32 slot() const
			{
				return m_slot;
			}
			ShaderResourceType type() const
			{
				return m_type;
			}
		private:
			rsl::vector<ResourceView*> m_views;
			ShaderResourceType m_type;
			s32 m_slot;
		};

		struct ShaderPipelineBindings
		{
			rsl::unordered_map<rsl::string, ViewParam> view_map;
			rsl::vector<ShaderResource> resources;
		};

		using ShaderParameter2 = ViewTable;
	}
}