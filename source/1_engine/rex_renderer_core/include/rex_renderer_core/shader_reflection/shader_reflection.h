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
		class ShaderSignature
		{
		public:
			ShaderSignature(ShaderType type)
				: m_shader_type(type)
			{}

			ShaderType shader_type() const
			{
				return m_shader_type;
			}

			const rsl::vector<ConstantBufferReflection>& constant_buffers() const
			{
				return m_constant_buffers;
			}
			const rsl::vector<ShaderParamReflection>& input_params() const
			{
				return m_input_params;
			}
			const rsl::vector<ShaderParamReflection>& output_params() const
			{
				return m_output_params;
			}
			const rsl::vector<BoundResourceReflection>& textures() const
			{
				return m_textures;
			}
			const rsl::vector<BoundResourceReflection>& samplers() const
			{
				return m_samplers;
			}

		protected:
			void init_constant_buffers(rsl::vector<ConstantBufferReflection>&& constantBuffers)
			{
				m_constant_buffers = rsl::move(constantBuffers);
			}
			void init_input_params(rsl::vector<ShaderParamReflection>&& inputParams)
			{
				m_input_params = rsl::move(inputParams);
			}
			void init_output_params(rsl::vector<ShaderParamReflection>&& outputParams)
			{
				m_output_params = rsl::move(outputParams);
			}
			void init_samplers(rsl::vector<BoundResourceReflection>&& samplers)
			{
				m_samplers = rsl::move(samplers);
			}
			void init_textures(rsl::vector<BoundResourceReflection>&& textures)
			{
				m_textures = rsl::move(textures);
			}

		private:
			rsl::vector<ConstantBufferReflection> m_constant_buffers;
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