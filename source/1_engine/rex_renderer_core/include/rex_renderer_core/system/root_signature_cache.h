#pragma once

#include "rex_renderer_core/resources/root_signature.h"
#include "rex_renderer_core/system/shader_pipeline.h"

namespace rex
{
	namespace gfx
	{
		namespace root_signature_cache
		{
			// Load a root signature from the cache, or store a new one
			RootSignature* load(const ShaderPipeline& pipeline);
			// Clear the cache
			void clear();
		}
	}
}