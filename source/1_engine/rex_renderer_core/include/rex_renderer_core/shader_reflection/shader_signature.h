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
		struct BoundResources
		{
			rsl::vector<BoundResourceReflection> constant_buffers;  // Holds all the textures of the shader
			rsl::vector<BoundResourceReflection> textures;					// Holds all the textures of the shader
			rsl::vector<BoundResourceReflection> samplers;					// Holds all the samplers of the shader
		};
		struct ShaderSignatureDesc
		{
			rsl::vector<CBufferReflDesc> constant_buffers;			// Holds all the constant buffers of the shader
			rsl::vector<ShaderParamReflection>   input_params;	// Holds all the input parameters of the shader
			rsl::vector<ShaderParamReflection>   output_params;	// Holds all the output parameters of the shader
			BoundResources bound_resources;											// Holds all bound resources of the shader
			rsl::tiny_stack_string shader_version;							// The version of the shader
			ShaderType type;																		// The type of the shader
		};

		// A signature describes all the resources that need to be bound for a shader
		class ShaderSignature
		{
		public:
			ShaderSignature() = default;
			ShaderSignature(ShaderType type);
			ShaderSignature(ShaderSignatureDesc&& desc);

			// The shader type this signature belongs to
			ShaderType shader_type() const;

			// The constant buffers of the shader
			const rsl::vector<CBufferReflDesc>& constant_buffers() const;
			// The input parameters of the shader
			const rsl::vector<ShaderParamReflection>& input_params() const;
			// The output parameters of the shader
			const rsl::vector<ShaderParamReflection>& output_params() const;
			// The constant buffers of the shader in resource format
			const rsl::vector<BoundResourceReflection>& constant_buffers_resources() const;
			// The textures of the shader
			const rsl::vector<BoundResourceReflection>& textures() const;
			// The samplers of the shader
			const rsl::vector<BoundResourceReflection>& samplers() const;

		private:
			ShaderSignatureDesc m_desc;
		};

		// Holds shader reflection data of each shader in the pipeleine
		struct ShaderPipelineReflection
		{
			ShaderSignature vs;
			ShaderSignature ps;
		};
		// Given a shader pipeline, create reflection data for each shader within the pipeline
		ShaderPipelineReflection reflect_shader_pipeline(const ShaderPipeline& shaderPipeline);

		namespace rhi
		{
			// Create reflection data of a single shader
			gfx::ShaderSignature reflect_shader(const gfx::Shader* shader);
		}
	}
}