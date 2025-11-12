#include "rex_engine/gfx/shader_reflection/shader_pipeline_reflection_builder.h"
#include "rex_engine/gfx/shader_reflection/view_table_builder.h"

#include "rex_std/bonus/utility/add_flag.h"

namespace rex
{
	namespace gfx
	{
		void ShaderPipelineReflectionBuilder::process_shader(const ShaderSignature& signature, ShaderVisibility visibility)
		{
			// Split of the shader parameters by register space
			ShaderResources splitted_babs = split_resources(signature.byte_address_buffers_resources());
			ShaderResources splitted_uabs = split_resources(signature.unordered_access_buffers_resources());
			ShaderResources splitted_cbs = split_resources(signature.constant_buffers_resources());
			ShaderResources splitted_textures = split_resources(signature.textures());
			ShaderResources splitted_samplers = split_resources(signature.samplers());

			add_bindings(splitted_babs.renderpass_resources, ShaderParameterType::ByteAddress, s_renderpass_register_space, visibility);
			add_bindings(splitted_uabs.renderpass_resources, ShaderParameterType::UnorderedAccessView, s_renderpass_register_space, visibility);
			add_bindings(splitted_cbs.renderpass_resources, ShaderParameterType::ConstantBuffer, s_renderpass_register_space, visibility);
			add_bindings(splitted_textures.renderpass_resources, ShaderParameterType::Texture, s_renderpass_register_space, visibility);
			add_bindings(splitted_samplers.renderpass_resources, ShaderParameterType::Sampler, s_renderpass_register_space, visibility);

			add_bindings(splitted_babs.material_resources, ShaderParameterType::ByteAddress, s_material_register_space, visibility);
			add_bindings(splitted_uabs.material_resources, ShaderParameterType::UnorderedAccessView, s_material_register_space, visibility);
			add_bindings(splitted_cbs.material_resources, ShaderParameterType::ConstantBuffer, s_material_register_space, visibility);
			add_bindings(splitted_textures.material_resources, ShaderParameterType::Texture, s_material_register_space, visibility);
			add_bindings(splitted_samplers.material_resources, ShaderParameterType::Sampler, s_material_register_space, visibility);
		}

		ShaderPipelineReflection ShaderPipelineReflectionBuilder::build()
		{
			return rsl::exchange(m_reflection_result, ShaderPipelineReflection());
		}

		ShaderResources ShaderPipelineReflectionBuilder::split_resources(const rsl::vector<ShaderResourceDeclaration> resources)
		{
			ShaderResources splitted_resources{};

			for (const auto& resource : resources)
			{
				switch (resource.register_space)
				{
				case s_material_register_space: splitted_resources.material_resources.push_back(resource); break;
				case s_renderpass_register_space: splitted_resources.renderpass_resources.push_back(resource); break;
				default: REX_ASSERT("Invalid constant buffer register space: {}", resource.register_space); break;
				}
			}

			return splitted_resources;
		}
		void ShaderPipelineReflectionBuilder::add_bindings(const rsl::vector<ShaderResourceDeclaration>& resources, ShaderParameterType type, s32 expectedRegisterSpace, ShaderVisibility visibility)
		{
			// The resources holds a list of each individual resource of a certain type, in a shader
			// We need to combine them into a set of ranges that are continious, based on register
			if (resources.empty())
			{
				return;
			}

			ShaderParametersStoreDesc* param_store_desc = nullptr;
			switch (expectedRegisterSpace)
			{
			case s_renderpass_register_space: param_store_desc = &m_reflection_result.renderpass_param_store_desc; break;
			case s_material_register_space: param_store_desc = &m_reflection_result.material_param_store_desc; break;
			}

			// Resource inputs will always be from the same shader, same type and register space
			// Chances are we have 1 continious range that can store all the resources
			// However, it is possible that resource registers are not continious
			// in which case we split the resources of in another range

			rsl::vector<ShaderResourceDeclaration> cb_views;
			rsl::vector<ShaderResourceDeclaration> other_views;

			for (const ShaderResourceDeclaration& resource : resources)
			{
				switch (resource.resource_type)
				{
				case ShaderParameterType::ConstantBuffer: cb_views.push_back(resource); break;
				default: other_views.push_back(resource); break;
				}
			}

			add_view_binding(param_store_desc, cb_views, type, expectedRegisterSpace, visibility);
			add_view_table_binding(param_store_desc, other_views, type, expectedRegisterSpace, visibility);
		}
		void ShaderPipelineReflectionBuilder::add_view_binding(ShaderParametersStoreDesc* paramStoreDesc, const rsl::vector<ShaderResourceDeclaration>& resources, ShaderParameterType type, s32 expectedRegisterSpace, ShaderVisibility visibility)
		{
			REX_UNUSED_PARAM(expectedRegisterSpace);

			if (resources.empty())
			{
				return;
			}

			// As we're processing a single view and not putting it in a table
			for (s32 i = 0; i < resources.size(); ++i)
			{
				const ShaderResourceDeclaration& resource = resources[i];

				if (paramStoreDesc->param_map.contains(resource.name))
				{
					s32 param_slot = paramStoreDesc->param_map.at(resource.name).slot;
					auto it = rsl::find_if(m_reflection_result.parameters.begin(), m_reflection_result.parameters.end(),
						[&](const ShaderParameterDeclaration& paramDecl)
						{
							return paramDecl.slot == param_slot;
						});

					REX_ASSERT_X(it != m_reflection_result.parameters.end(), "found existing parameter with name '{}', but not found parameter in list for root signature", resource.name);
					rsl::add_flag(it->visibility, visibility);

					continue;
				}

				s32 slot = m_reflection_result.parameters.size();
				s32 idx = paramStoreDesc->shader_resource_descs.size();

				REX_ASSERT_X(resource.register_space == expectedRegisterSpace, "Unexpected register space of resource. space: {} expected: {}", resource.register_space, expectedRegisterSpace);
				ViewOffset view_offset{};
				paramStoreDesc->param_map.emplace(resource.name, ShaderParameterLocation{ slot, idx, view_offset });
				ViewRangeDeclaration view_range = ViewRangeDeclaration(resource.shader_register, 1, type, resource.register_space);
				const auto& view_table = m_reflection_result.parameters.emplace_back(ShaderParameterDeclaration(slot, { view_range }, 1, type, visibility));
				paramStoreDesc->shader_resource_descs.push_back(ShaderParameterDesc{ type, slot, view_table.total_num_views });
			}

		}
		void ShaderPipelineReflectionBuilder::add_view_table_binding(ShaderParametersStoreDesc* paramStoreDesc, const rsl::vector<ShaderResourceDeclaration>& resources, ShaderParameterType type, s32 expectedRegisterSpace, ShaderVisibility visibility)
		{
			if (resources.empty())
			{
				return;
			}

			ViewTableBuilder view_table_builder(expectedRegisterSpace, type);
			s32 slot = m_reflection_result.parameters.size();
			s32 idx = paramStoreDesc->shader_resource_descs.size();

			for (s32 i = 0; i < resources.size(); ++i)
			{
				const ShaderResourceDeclaration& resource = resources[i];
				ViewOffset view_offset = view_table_builder.add_resource(resource);
				paramStoreDesc->param_map.emplace(resource.name, ShaderParameterLocation{ slot, idx, view_offset });
			}

			const auto& view_table = m_reflection_result.parameters.emplace_back(view_table_builder.build(slot, visibility));
			paramStoreDesc->shader_resource_descs.push_back(ShaderParameterDesc{ type, slot, view_table.total_num_views });
		}
	}
}