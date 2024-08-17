#include "rex_renderer_core/shader_reflection/shader_reflection_cache.h"

#include "rex_std/unordered_map.h"
#include "rex_std/bonus/utility.h"

#include "rex_renderer_core/shader_reflection/shader_signature.h"
#include "rex_renderer_core/shader_reflection/bound_resource_reflection.h"
#include "rex_renderer_core/gfx/shader_visibility.h"


namespace rex
{
	namespace gfx
	{
		namespace shader_reflection_cache
		{
			rsl::unordered_map<ShaderPipeline, ShaderPipelineReflection> g_reflection_cache_lookup;
			const s32 g_renderpass_register_space = 0;
			const s32 g_material_register_space = 1;

			struct SplittedResources
			{
				rsl::vector<ShaderResourceDeclaration> material_resources;
				rsl::vector<ShaderResourceDeclaration> renderpass_resources;
			};

			class ViewTableBuilder
			{
			public:
				ViewTableBuilder(s32 registerSpace, ShaderParameterType expectedResourceType)
					: m_start_register(0)
					, m_expected_register(0)
					, m_total_num_views(0)
					, m_expected_register_space(registerSpace)
					, m_expected_resource_type(expectedResourceType)
				{}

				ViewOffset add_resource(s32 slot, const ShaderResourceDeclaration& resource)
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
					view_offset.range_offset = m_ranges.size();
					view_offset.offset_within_range = resource.shader_register - m_start_register;

					return view_offset;
				}

				ShaderParameterDeclaration build(s32 slot, ShaderVisibility visibility)
				{
					add_new_range();
					return ShaderParameterDeclaration(slot, rsl::move(m_ranges), m_total_num_views, m_expected_resource_type, visibility);
				}

			private:
				void add_new_range()
				{
					s32 num_views = m_expected_register - m_start_register;
					if (num_views > 0)
					{
						m_total_num_views += num_views;
						m_ranges.emplace_back(m_start_register, num_views, m_expected_resource_type, m_expected_register_space);
					}
				}

			private:
				s32 m_start_register;
				s32 m_expected_register;
				s32 m_expected_register_space;
				s32 m_total_num_views;
				ShaderParameterType m_expected_resource_type;
				rsl::vector<ViewRangeDeclaration> m_ranges;
			};

			class ShaderPipelineReflectionBuilder
			{
			public:
				void process_shader(const ShaderSignature& signature, ShaderVisibility visibility)
				{
					// Split of the shader parameters by register space
					SplittedResources splitted_cbs = split_resources(signature.constant_buffers_resources());
					SplittedResources splitted_textures = split_resources(signature.textures());
					SplittedResources splitted_samplers = split_resources(signature.samplers());

					add_bindings(splitted_cbs.renderpass_resources, ShaderParameterType::ConstantBuffer, g_renderpass_register_space, visibility);
					add_bindings(splitted_textures.renderpass_resources, ShaderParameterType::Texture, g_renderpass_register_space, visibility);
					add_bindings(splitted_samplers.renderpass_resources, ShaderParameterType::Sampler, g_renderpass_register_space, visibility);

					add_bindings(splitted_cbs.material_resources, ShaderParameterType::ConstantBuffer, g_material_register_space, visibility);
					add_bindings(splitted_textures.material_resources, ShaderParameterType::Texture, g_material_register_space, visibility);
					add_bindings(splitted_samplers.material_resources, ShaderParameterType::Sampler, g_material_register_space, visibility);
				}

				ShaderPipelineReflection build()
				{
					return rsl::move(m_reflection_result);
				}

			private:
				SplittedResources split_resources(const rsl::vector<ShaderResourceDeclaration> resources)
				{
					SplittedResources splitted_resources{};

					for (const auto& resource : resources)
					{
						switch (resource.register_space)
						{
						case g_material_register_space: splitted_resources.material_resources.push_back(resource); break;
						case g_renderpass_register_space: splitted_resources.renderpass_resources.push_back(resource); break;
						default: REX_ASSERT("Invalid constant buffer register space: {}", resource.register_space); break;
						}
					}

					return splitted_resources;
				}
				void add_bindings(const rsl::vector<ShaderResourceDeclaration>& resources, ShaderParameterType type, s32 expectedRegisterSpace, ShaderVisibility visibility)
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
					case g_renderpass_register_space: param_store_desc = &m_reflection_result.renderpass_param_store_desc; break;
					case g_material_register_space: param_store_desc = &m_reflection_result.material_param_store_desc; break;
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
				void add_view_binding(ShaderParametersStoreDesc* paramStoreDesc, const rsl::vector<ShaderResourceDeclaration>& resources, ShaderParameterType type, s32 expectedRegisterSpace, ShaderVisibility visibility)
				{
					if (resources.empty())
					{
						return;
					}

					// As we're processing a single view and not putting it in a table
					for (s32 i = 0; i < resources.size(); ++i)
					{
						s32 slot = m_reflection_result.parameters.size();
						s32 idx = paramStoreDesc->shader_resource_descs.size();

						const ShaderResourceDeclaration& resource = resources[i];
						ViewOffset view_offset{};
						paramStoreDesc->param_map.emplace(resource.name, ShaderParameterLocation{ slot, idx, view_offset });
						ViewRangeDeclaration view_range = ViewRangeDeclaration(resource.shader_register, 1, type, resource.register_space);
						const auto& view_table = m_reflection_result.parameters.emplace_back(ShaderParameterDeclaration(slot, { view_range }, 1, type, visibility));
						paramStoreDesc->shader_resource_descs.push_back(ShaderParameterDesc{ type, slot, view_table.total_num_views });
					}

				}
				void add_view_table_binding(ShaderParametersStoreDesc* paramStoreDesc, const rsl::vector<ShaderResourceDeclaration>& resources, ShaderParameterType type, s32 expectedRegisterSpace, ShaderVisibility visibility)
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
						ViewOffset view_offset = view_table_builder.add_resource(slot, resource);
						paramStoreDesc->param_map.emplace(resource.name, ShaderParameterLocation{ slot, idx, view_offset });
					}

					const auto& view_table = m_reflection_result.parameters.emplace_back(view_table_builder.build(slot, visibility));
					paramStoreDesc->shader_resource_descs.push_back(ShaderParameterDesc{ type, slot, view_table.total_num_views });
				}

			private:
				ShaderPipelineReflection m_reflection_result;
			};

			const ShaderPipelineReflection& load(const ShaderPipeline& pipeline)
			{
				if (g_reflection_cache_lookup.contains(pipeline))
				{
					return g_reflection_cache_lookup.at(pipeline);
				}

				ShaderPipelineReflectionBuilder builder{};
				builder.process_shader(api::reflect_shader(pipeline.vs), ShaderVisibility::Vertex);
				builder.process_shader(api::reflect_shader(pipeline.ps), ShaderVisibility::Pixel);

				ShaderPipelineReflection reflection = builder.build();
				auto result = g_reflection_cache_lookup.emplace(pipeline, rsl::move(reflection));
				return result.inserted_element->value;
			}
		}
	}
}