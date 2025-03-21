#include "rex_engine/gfx/shader_reflection/shader_signature.h"

namespace rex
{
	namespace gfx
	{
		ShaderSignature::ShaderSignature(ShaderSignatureDesc&& desc)
		: m_desc(rsl::move(desc))
		{}

		// The shader type this signature belongs to
		ShaderType ShaderSignature::shader_type() const
		{
			return m_desc.type;
		}

		// The constant buffers of the shader
		const rsl::vector<ConstantBufferDeclaration>& ShaderSignature::constant_buffers() const
		{
			return m_desc.constant_buffers;
		}
		// The input parameters of the shader
		const rsl::vector<ShaderIODeclaration>& ShaderSignature::input_params() const
		{
			return m_desc.input_params;
		}
		// The output parameters of the shader
		const rsl::vector<ShaderIODeclaration>& ShaderSignature::output_params() const
		{
			return m_desc.output_params;
		}
		// The byte address buffers of the shader in resource format
		const rsl::vector<ShaderResourceDeclaration>& ShaderSignature::byte_address_buffers_resources() const
		{
			return m_desc.bound_resources.byte_address_buffers;
		}
		// The unordered access buffers of the shader in resource format
		const rsl::vector<ShaderResourceDeclaration>& ShaderSignature::unordered_access_buffers_resources() const
		{
			return m_desc.bound_resources.unordered_access_buffers;
		}
		// The constant buffers of the shader in resource format
		const rsl::vector<ShaderResourceDeclaration>& ShaderSignature::constant_buffers_resources() const
		{
			return m_desc.bound_resources.constant_buffers;
		}
		// The textures of the shader
		const rsl::vector<ShaderResourceDeclaration>& ShaderSignature::textures() const
		{
			return m_desc.bound_resources.textures;
		}
		// The samplers of the shader
		const rsl::vector<ShaderResourceDeclaration>& ShaderSignature::samplers() const
		{
			return m_desc.bound_resources.samplers;
		}
	}
}