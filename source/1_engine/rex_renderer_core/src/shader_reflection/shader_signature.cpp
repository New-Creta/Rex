#include "rex_renderer_core/shader_reflection/shader_signature.h"

namespace rex
{
	namespace gfx
	{
		ShaderSignature(ShaderSignatureDesc&& desc);
		: m_desc(rsl::move(desc))
		{}

		// The shader type this signature belongs to
		ShaderType ShaderSignature::shader_type() const
		{
			return m_desc.shader_type;
		}

		// The constant buffers of the shader
		const rsl::vector<CBufferReflDesc>& ShaderSignature::constant_buffers() const
		{
			return m_desc.constant_buffers;
		}
		// The input parameters of the shader
		const rsl::vector<ShaderParamReflection>& ShaderSignature::input_params() const
		{
			return m_desc.input_params;
		}
		// The output parameters of the shader
		const rsl::vector<ShaderParamReflection>& ShaderSignature::output_params() const
		{
			return m_desc.output_params;
		}
		// The textures of the shader
		const rsl::vector<BoundResourceReflection>& ShaderSignature::textures() const
		{
			return m_desc.textures;
		}
		// The samplers of the shader
		const rsl::vector<BoundResourceReflection>& ShaderSignature::samplers() const
		{
			return m_desc.samplers;
		}

		ShaderPipelineReflection reflect_shader_pipeline(const ShaderPipeline& shaderPipeline)
		{
			ShaderPipelineReflection refl{};

			refl.vs = rex::gfx::rhi::reflect_shader(shaderPipeline.vs.get());
			refl.ps = rex::gfx::rhi::reflect_shader(shaderPipeline.ps.get());

			return refl;
		}
	}
}