#pragma once

#include "rex_engine/gfx/resources/root_signature.h"
#include "rex_engine/gfx/system/shader_pipeline.h"

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