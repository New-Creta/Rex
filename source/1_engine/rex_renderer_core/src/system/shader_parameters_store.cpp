#include "rex_renderer_core/system/shader_parameters_store.h"

#include "rex_renderer_core/resources/constant_buffer.h"
#include "rex_renderer_core/resources/texture_2d.h"
#include "rex_renderer_core/resources/sampler_2d.h"

namespace rex
{
	namespace gfx
	{
		ShaderParametersStore::ShaderParametersStore(const ShaderParametersStoreDesc& desc)
			: m_param_to_location_lookup(&desc.param_map)
		{
			m_shader_resources.reserve(desc.shader_resource_descs.size());
			for (ShaderParameterDesc shaderResourceDesc : desc.shader_resource_descs)
			{
				switch (shaderResourceDesc.type)
				{
				case ShaderParameterType::ConstantBuffer: m_shader_resources.push_back(rsl::make_unique<ViewShaderParam>(shaderResourceDesc)); break;
				default: m_shader_resources.push_back(rsl::make_unique<ViewTableShaderParam>(shaderResourceDesc)); break;
				}
			}
		}

		void ShaderParametersStore::set(rsl::string_view name, ConstantBuffer* cb)
		{
			ShaderParameterLocation loc = m_param_to_location_lookup->at(name);
			m_shader_resources[loc.idx]->update_view(loc.view_offset, cb);
		}
		void ShaderParametersStore::set(rsl::string_view name, Texture2D* texture)
		{
			ShaderParameterLocation loc = m_param_to_location_lookup->at(name);
			m_shader_resources[loc.idx]->update_view(loc.view_offset, texture);
		}
		void ShaderParametersStore::set(rsl::string_view name, Sampler2D* sampler)
		{
			ShaderParameterLocation loc = m_param_to_location_lookup->at(name);
			m_shader_resources[loc.idx]->update_view(loc.view_offset, sampler);
		}
		ShaderParameterLocation ShaderParametersStore::location(rsl::string_view name) const
		{
			return m_param_to_location_lookup->at(name);
		}

		const rsl::vector<rsl::unique_ptr<ShaderParameter>>& ShaderParametersStore::params() const
		{
			return m_shader_resources;
		}
	}
}