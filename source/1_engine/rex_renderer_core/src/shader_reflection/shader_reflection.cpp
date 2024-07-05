#include "rex_renderer_core/shader_reflection/shader_reflection.h"

namespace rex
{
	namespace gfx
	{
		ShaderPipelineReflection reflect_shader_pipeline(const ShaderPipeline& shaderPipeline)
		{
			ShaderPipelineReflection refl{};

			refl.vs = rex::gfx::rhi::reflect_shader(shaderPipeline.vs.get());
			refl.ps = rex::gfx::rhi::reflect_shader(shaderPipeline.ps.get());

			return refl;
		}
	}
}