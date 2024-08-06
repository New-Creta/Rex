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
#include "rex_renderer_core/system/param_binding_indices.h"
#include "rex_renderer_core/system/view_param.h"
#include "rex_renderer_core/system/view_table.h"

#include "rex_std/bonus/utility/output_param.h"

namespace rex
{
	namespace gfx
	{
		class ResourceView;

		//// Describes a range of views
		//struct ViewRange
		//{
		//	ViewRange(s32 baseRegister, s32 numViews, ShaderResourceType type, s32 registerSpace)
		//		: base_register(baseRegister)
		//		, num_views(numViews)
		//		, type(type)
		//		, register_space(registerSpace)
		//	{}

		//	s32 base_register;
		//	s32 num_views;
		//	ShaderResourceType type;
		//	s32 register_space;
		//};

		//// Describes a list of view ranges
		//struct ViewTable
		//{
		//	ViewTable(rsl::vector<ViewRange>&& ranges, ShaderVisibility shaderVis)
		//		: ranges(rsl::move(ranges))
		//		, visibility(shaderVis)
		//	{}

		//	rsl::vector<ViewRange> ranges;
		//	ShaderVisibility visibility;
		//};

	
		//class ViewTableBuilder
		//{
		//public:
		//	ViewTableBuilder(s32 registerSpace, ShaderResourceType expectedResourceType)
		//		: m_start_register(0)
		//		, m_expected_register(0)
		//		, m_expected_register_space(registerSpace)
		//		, m_expected_resource_type(expectedResourceType)
		//	{}

		//	ViewOffset add_resource(s32 slot, const BoundResourceReflection& resource)
		//	{
		//		REX_ASSERT_X(resource.register_space == m_expected_register_space, "Invalid register space for resource. expected: {} actual: {} resource: {}", m_expected_register, resource.register_space, resource.name);
		//		REX_ASSERT_X(resource.resource_type == m_expected_resource_type, "Invalid resource type for resource. expected: {}, actual: {}, resource: {}", rsl::enum_refl::enum_name(m_expected_resource_type), rsl::enum_refl::enum_name(resource.resource_type), resource.name);

		//		if (resource.shader_register != m_expected_register)
		//		{
		//			s32 num_views = m_expected_register - m_start_register;
		//			if (num_views > 0)
		//			{
		//				m_ranges.emplace_back(m_start_register, num_views, m_expected_resource_type, m_expected_register_space);
		//			}

		//			m_start_register = resource.shader_register;
		//		}

		//		m_expected_register = resource.shader_register + 1;

		//		ViewOffset view_offset{};
		//		view_offset.range_offset = m_ranges.size();
		//		view_offset.offset_within_range = resource.shader_register - m_start_register;

		//		return view_offset;
		//	}

		//	ViewTable build(ShaderVisibility visibility)
		//	{
		//		return ViewTable(rsl::move(m_ranges), visibility);
		//	}

		//private:
		//	s32 m_start_register;
		//	s32 m_expected_register;
		//	s32 m_expected_register_space;
		//	ShaderResourceType m_expected_resource_type;
		//	rsl::vector<ViewRange> m_ranges;
		//};

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

		//struct ShaderParameterLocation
		//{
		//	s32 idx;
		//	ViewOffset view_offset;
		//};
		//// Should hold the map from resource name to 2D coordinate where in the resources it should go
		//class ShaderParameterStoreLookupBuilder
		//{
		//public:
		//	void add_resource(const BoundResourceReflection& resource, s32 slot, ViewOffset viewOffset)
		//	{
		//		m_name_to_location.emplace(resource.name, ShaderParameterLocation{slot, viewOffset});
		//	}

		//private:
		//	rsl::unordered_map<rsl::string, ShaderParameterLocation> m_name_to_location;
		//};

		// The shader pipeline parameters class is a structure holding all parameters needed for a shader pipeline
		// You can think of this as the root signature in DirectX 12
		// It'll construct a list of slots where each slot is tied to an input parameter
		// Parameters are processed per shader, per binding slot. 
		// This means that slots will be laid out like this
		// [0] - VS - Material Param
		// [1] - VS - Material Param
		// [2] - VS - Render Pass Param
		// [3] - VS - Render Pass Param
		// [4] - PS - Material Param
		// [5] - PS - Material Param
		// [6] - PS - Render Pass Param
		// [7] - PS - Render Pass Param
		//class ShaderPipelineParameters
		//{
		//public:
		//	ShaderPipelineParameters(const ShaderPipeline& shaderPipeline);

		//	s32 num() const;
		//	ShaderParameter2* operator[](s32 idx);
		//	const ShaderParameter2* operator[](s32 idx) const;

		//	const rsl::unordered_map<ShaderType, rsl::vector<ViewTable>>& view_tables_per_shader() const;

		//	ShaderPipelineBindings params(ShaderParameterBinding binding) const;

		//	const rsl::unordered_map<ShaderType, ParamBindingSlots>& shader_param_binding_slots() const;

		//private:
		//	s32 process_shader(s32 startSlot, const ShaderSignature& signature, ShaderVisibility visibility);

		//	struct SplittedResources
		//	{
		//		rsl::vector<BoundResourceReflection> material_resources;
		//		rsl::vector<BoundResourceReflection> renderpass_resources;
		//	};
		//	SplittedResources split_resources(const rsl::vector<BoundResourceReflection> resources);
		//	void add_bindings(const rsl::vector<BoundResourceReflection>& resources, ShaderPipelineBindings& targetBindings, ShaderResourceType type, s32 expectedRegisterSpace, ShaderVisibility visibility);

		//private:
		//	// Holds all the parameters that are needed for the shader pipeline
		//	// This owns all the parameters but is not meant to be accessed directly
		//	rsl::vector<rsl::unique_ptr<ShaderParameter2>> m_parameters;

		//	// Holds all the parameters that need to be set by a material
		//	// It's a cache, and does not own these parameters
		//	// The pointers in here are owned by the above vector
		//	ShaderParameterStoreLookupBuilder m_material_parameters;

		//	// Holds all the parameters that need to be set by a renderpass
		//	// It's a cache, and does not own these parameters
		//	// The pointers in here are owned by the above vector
		//	ShaderParameterStoreLookupBuilder m_renderpass_parameters;










		//	ShaderPipelineBindings m_material_bindings;
		//	ShaderPipelineBindings m_renderpass_bindings;
		//	rsl::unordered_map<ShaderType, ParamBindingSlots> m_shader_type_to_binding_slots;
		//	rsl::unordered_map<ShaderType, rsl::vector<ViewTable>> m_view_tables_per_shader;
		//};

		
	}
}