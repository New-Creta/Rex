#include "rex_renderer_core/shader_reflection/shader_parameter_store_cache.h"

#include "rex_renderer_core/shader_reflection/shader_param_lookup_cache.h"

namespace rex
{
	namespace gfx
	{
		namespace shader_parameter_store_cache
		{
			rsl::unordered_map<ShaderPipeline, ShaderParametersStoreDesc> g_renderpass_params_desc_cache;
			rsl::unordered_map<ShaderPipeline, ShaderParametersStoreDesc> g_material_params_desc_cache;

			rsl::unique_ptr<ShaderParametersStore> construct_new(ShaderParameterBinding binding, const ShaderPipeline& pipeline)
			{
				rsl::unordered_map<ShaderPipeline, ShaderParametersStoreDesc>* cache = nullptr;
				switch (binding)
				{
				case rex::gfx::ShaderParameterBinding::RenderPass: cache = &g_renderpass_params_desc_cache; break;
				case rex::gfx::ShaderParameterBinding::Material: cache = &g_material_params_desc_cache; break;
				}
				
				if (cache->contains(pipeline))
				{
					const ShaderParametersStoreDesc& desc = cache->at(pipeline);
					return rsl::make_unique<ShaderParametersStore>(desc);
				}

				//ShaderPipelineParameters& parameters = shader_parameters_cache::load(pipeline);


				//ShaderParametersStoreDesc desc{};
				
				
				
				return nullptr;
			}

			rsl::unique_ptr<ShaderParametersStore> load(const ShaderPipeline& pipeline, ShaderParameterBinding binding)
			{
				switch (binding)
				{
				case rex::gfx::ShaderParameterBinding::RenderPass: return construct_new(binding, pipeline);
				case rex::gfx::ShaderParameterBinding::Material:	 return construct_new(binding, pipeline);
				}

				return nullptr;
			}
		}
	}
}