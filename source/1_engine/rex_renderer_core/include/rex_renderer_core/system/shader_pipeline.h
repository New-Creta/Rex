#pragma once

#include "rex_renderer_core/resources/shader.h"
#include "rex_std/functional.h"

namespace rex
{
	namespace gfx
	{
		class Shader;

		// A shader pipeline object holds all the shaders will be used in a pipeline state
		struct ShaderPipeline
		{
			Shader* vs; // The vertex shader
			Shader* ps; // The pixel shader
		};

		bool operator==(const ShaderPipeline& lhs, const ShaderPipeline& rhs);
		bool operator!=(const ShaderPipeline& lhs, const ShaderPipeline& rhs);
	}
}

namespace rsl
{
	inline namespace v1
	{
		template<>
		struct hash<rex::gfx::ShaderPipeline>
		{
			rsl::hash_result operator()(const rex::gfx::ShaderPipeline& pipeline) const
			{
				uint64 seed = 0;
				seed = rsl::internal::hash_combine(seed, rsl::comp_hash(pipeline.vs));
				seed = rsl::internal::hash_combine(seed, rsl::comp_hash(pipeline.ps));

				return static_cast<hash_result>(seed);
			}
		};
	}
}