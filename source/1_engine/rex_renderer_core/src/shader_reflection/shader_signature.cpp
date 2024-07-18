#include "rex_renderer_core/shader_reflection/shader_signature.h"

namespace rex
{
	namespace gfx
	{
		ShaderSignature::ShaderSignature(ShaderType type)
			: m_shader_type(type)
		{}

		// The shader type this signature belongs to
		ShaderType ShaderSignature::shader_type() const
		{
			return m_shader_type;
		}

		// The constant buffers of the shader
		const rsl::vector<CBufferReflDesc>& ShaderSignature::constant_buffers() const
		{
			return m_constant_buffers;
		}
		// The input parameters of the shader
		const rsl::vector<ShaderParamReflection>& ShaderSignature::input_params() const
		{
			return m_input_params;
		}
		// The output parameters of the shader
		const rsl::vector<ShaderParamReflection>& ShaderSignature::output_params() const
		{
			return m_output_params;
		}
		// The textures of the shader
		const rsl::vector<BoundResourceReflection>& ShaderSignature::textures() const
		{
			return m_textures;
		}
		// The samplers of the shader
		const rsl::vector<BoundResourceReflection>& ShaderSignature::samplers() const
		{
			return m_samplers;
		}

		void ShaderSignature::init_constant_buffers(rsl::vector<CBufferReflDesc>&& constantBuffers)
		{
			m_constant_buffers = rsl::move(constantBuffers);
		}
		void ShaderSignature::init_input_params(rsl::vector<ShaderParamReflection>&& inputParams)
		{
			m_input_params = rsl::move(inputParams);
		}
		void ShaderSignature::init_output_params(rsl::vector<ShaderParamReflection>&& outputParams)
		{
			m_output_params = rsl::move(outputParams);
		}
		void ShaderSignature::init_samplers(rsl::vector<BoundResourceReflection>&& samplers)
		{
			m_samplers = rsl::move(samplers);
		}
		void ShaderSignature::init_textures(rsl::vector<BoundResourceReflection>&& textures)
		{
			m_textures = rsl::move(textures);
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