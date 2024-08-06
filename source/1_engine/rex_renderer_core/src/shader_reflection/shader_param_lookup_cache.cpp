//#include "rex_renderer_core/shader_reflection/shader_param_lookup_cache.h"
//
//#include "rex_std/functional.h"
//#include "rex_renderer_core/system/shader_pipeline.h"
//
//namespace rex
//{
//	namespace gfx
//	{
//		rsl::unordered_map<ShaderPipeline, ShaderPipelineParameters&> g_params_lookup_cache;
//
//		namespace shader_parameters_cache
//		{
//			ShaderPipelineParameters& load(const ShaderPipeline& pipeline)
//			{
//				if (g_params_lookup_cache.contains(pipeline))
//				{
//					return g_params_lookup_cache.at(pipeline);
//				}
//
//				auto res = g_params_lookup_cache.emplace(pipeline, pipeline);
//				return res.inserted_element->value;
//			}
//		}
//	}
//}