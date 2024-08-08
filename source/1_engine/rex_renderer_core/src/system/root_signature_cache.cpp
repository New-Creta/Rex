#include "rex_renderer_core/system/root_signature_cache.h"

#include "rex_renderer_core/gfx/rhi.h"

#include "rex_std/functional.h"
#include "rex_std/unordered_map.h"

#include "rex_renderer_core/shader_reflection/shader_reflection_cache.h"

namespace rex
{
	namespace gfx
	{
		namespace root_signature_cache
		{
			rsl::unordered_map<ShaderPipeline, rsl::unique_ptr<RootSignature>> g_root_sig_cache;

			RootSignature* load(const ShaderPipeline& pipeline)
			{
				auto it = g_root_sig_cache.find(pipeline);
				if (it != g_root_sig_cache.end())
				{
					return it->value.get();
				}

				ShaderPipelineReflection& reflection = shader_reflection_cache::load(pipeline);
				rsl::unique_ptr<RootSignature> root_sig = rhi::create_root_signature(reflection.parameters);
				return g_root_sig_cache.emplace(pipeline, rsl::move(root_sig)).inserted_element->value.get();
			}

			void clear()
			{
				g_root_sig_cache.clear();
			}
		}
	}
}