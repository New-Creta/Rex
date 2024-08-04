#pragma once

#include "rex_std/vector.h"
#include "rex_engine/engine/types.h"
#include "rex_std/unordered_map.h"
#include "rex_std/bonus/string.h"

#include "rex_renderer_core/resources/resource.h"
#include "rex_renderer_core/system/shader_elements.h"

#include "rex_renderer_core/shader_reflection/bound_resource_reflection.h"
#include "rex_renderer_core/shader_reflection/shader_signature.h"
#include "rex_renderer_core/system/param_binding_indices.h"
#include "rex_renderer_core/system/view_param.h"

namespace rex
{
	namespace gfx
	{
		struct ViewRange
		{
			ViewRange(s32 baseRegister, s32 numViews, DescriptorRangeType type, s32 registerSpace)
				: base_register(baseRegister)
				, num_views(numViews)
				, type(type)
				, register_space(registerSpace)
			{}

			s32 base_register;
			s32 num_views;
			DescriptorRangeType type;
			s32 register_space;
		};

		struct ViewTable
		{
			ViewTable(ShaderVisibility shaderVis)
				: visibility(shaderVis)
			{}

			rsl::vector<ViewRange> ranges;
			ShaderVisibility visibility;
		};

		enum class ParamBindings
		{
			MaterialConstantBuffers,
			MaterialTextures,
			MaterialSamplers,
			RenderPassConstantBuffers,
			RenderPassTextures,
			RenderPassSamplers
		};

		class ShaderPipelineParameters
		{
		public:
			ShaderPipelineParameters(const ShaderPipeline& shaderPipeline);

			//const rsl::vector<ViewTable>& material_view_tables() const;
			//const rsl::vector<ViewTable>& renderpass_view_tables() const;
			const rsl::unordered_map<ShaderType, rsl::vector<ViewTable>>& view_tables_per_shader() const;

			rsl::unordered_map<rsl::string, ViewParam> material_params();
			rsl::unordered_map<rsl::string, ViewParam> renderpass_params();

			ParamBindingIndices shader_param_binding_slots(ShaderType type) const;

		private:
			s32 process_shader(s32 slotIdx, const ShaderSignature& signature, ShaderVisibility visibility);

			struct SplittedResources
			{
				rsl::vector<BoundResourceReflection> material_resources;
				rsl::vector<BoundResourceReflection> renderpass_resources;
			};
			SplittedResources split_resources(const rsl::vector<BoundResourceReflection> resources);
			rsl::vector<ViewRange> create_resource_view_ranges(const rsl::vector<BoundResourceReflection>& resources, rsl::unordered_map<rsl::string, ViewParam>& nameToIdxMap, DescriptorRangeType type, s32 expectedRegisterSpace);

		private:
			//rsl::vector<ViewTable> m_material_view_tables;
			//rsl::vector<ViewTable> m_renderpass_view_tables;
			rsl::unordered_map<rsl::string, ViewParam> m_material_param_name_to_idx;
			rsl::unordered_map<rsl::string, ViewParam> m_renderpass_param_name_to_idx;
			rsl::unordered_map<ShaderType, ParamBindingIndices> m_shader_type_to_binding_indices;
			rsl::unordered_map<ShaderType, rsl::vector<ViewTable>> m_view_tables_per_shader;
		};

		
	}
}