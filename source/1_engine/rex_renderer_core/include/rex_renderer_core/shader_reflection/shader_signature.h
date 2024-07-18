#pragma once

#include "rex_renderer_core/system/shader_type.h"

#include "rex_renderer_core/shader_reflection/bound_resource_reflection.h"
#include "rex_renderer_core/shader_reflection/constant_buffer_reflection.h"
#include "rex_renderer_core/shader_reflection/shader_param_reflection.h"

#include "rex_renderer_core/system/shader_pipeline.h"

namespace rex
{
	namespace gfx
	{
		// A signature describes all the resources that need to be bound for a shader
		class ShaderSignature
		{
		public:
			ShaderSignature(ShaderType type);

			// The shader type this signature belongs to
			ShaderType shader_type() const;

			// The constant buffers of the shader
			const rsl::vector<CBufferReflDesc>& constant_buffers() const;
			// The input parameters of the shader
			const rsl::vector<ShaderParamReflection>& input_params() const;
			// The output parameters of the shader
			const rsl::vector<ShaderParamReflection>& output_params() const;
			// The textures of the shader
			const rsl::vector<BoundResourceReflection>& textures() const;
			// The samplers of the shader
			const rsl::vector<BoundResourceReflection>& samplers() const;

		protected:
			void init_constant_buffers(rsl::vector<CBufferReflDesc>&& constantBuffers);
			void init_input_params(rsl::vector<ShaderParamReflection>&& inputParams);
			void init_output_params(rsl::vector<ShaderParamReflection>&& outputParams);
			void init_samplers(rsl::vector<BoundResourceReflection>&& samplers);
			void init_textures(rsl::vector<BoundResourceReflection>&& textures);
		private:
			rsl::vector<CBufferReflDesc> m_constant_buffers;
			rsl::vector<ShaderParamReflection>   m_input_params;
			rsl::vector<ShaderParamReflection>   m_output_params;
			rsl::vector<BoundResourceReflection> m_samplers;
			rsl::vector<BoundResourceReflection> m_textures;

			ShaderType m_shader_type;
		};

		struct ShaderPipelineReflection
		{
			rsl::unique_ptr<ShaderSignature> vs;
			rsl::unique_ptr<ShaderSignature> ps;
		};
		ShaderPipelineReflection reflect_shader_pipeline(const ShaderPipeline& shaderPipeline);

		namespace rhi
		{
			rsl::unique_ptr<gfx::ShaderSignature> reflect_shader(const gfx::Shader* shader);
		}
	}
}