#include "rex_renderer_core/system/shader_parameters_store.h"

#include "rex_renderer_core/resources/constant_buffer.h"
#include "rex_renderer_core/resources/texture_2d.h"
#include "rex_renderer_core/resources/sampler_2d.h"

namespace rex
{
	namespace gfx
	{
		ShaderParameter::ShaderParameter(ShaderParameterType2 type, ShaderType owningShaderType)
			: m_type(type)
			, m_owning_shader_type(owningShaderType)
			, m_resource(nullptr)
		{
		}

		void ShaderParameter::set_resource(Resource* resource)
		{
			m_resource = resource;
		}
		Resource* ShaderParameter::resource()
		{
			return m_resource;
		}
		ShaderType ShaderParameter::owning_shader_type() const
		{
			return m_owning_shader_type;
		}

		ShaderParametersStore::ShaderParametersStore(const ShaderParametersStoreDesc& desc)
			: m_param_to_location_lookup(&desc.param_map)
		{
			m_shader_resources.reserve(desc.shader_resource_descs.size());
			for (ShaderResourceDesc shaderResourceDesc : desc.shader_resource_descs)
			{
				m_shader_resources.emplace_back(shaderResourceDesc);
			}
		}

		void ShaderParametersStore::set(rsl::string_view name, ConstantBuffer* cb)
		{
			ShaderParameterLocation loc = m_param_to_location_lookup->at(name);
			m_shader_resources[loc.idx].update_view(loc.view_offset, reinterpret_cast<ResourceView*>(cb));
		}
		void ShaderParametersStore::set(rsl::string_view name, Texture2D* texture)
		{
			ShaderParameterLocation loc = m_param_to_location_lookup->at(name);
			m_shader_resources[loc.idx].update_view(loc.view_offset, texture->resource_view());
		}
		void ShaderParametersStore::set(rsl::string_view name, Sampler2D* sampler)
		{
			ShaderParameterLocation loc = m_param_to_location_lookup->at(name);
			m_shader_resources[loc.idx].update_view(loc.view_offset, sampler->resource_view());
		}
		ShaderParameterLocation ShaderParametersStore::location(rsl::string_view name) const
		{
			return m_param_to_location_lookup->at(name);
		}

		const rsl::vector<ShaderResource>& ShaderParametersStore::params() const
		{
			return m_shader_resources;
		}
	}
}