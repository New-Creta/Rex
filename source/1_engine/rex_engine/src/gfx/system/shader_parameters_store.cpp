#include "rex_engine/gfx/system/shader_parameters_store.h"

#include "rex_engine/gfx/resources/constant_buffer.h"
#include "rex_engine/gfx/resources/texture_2d.h"
#include "rex_engine/gfx/resources/sampler_2d.h"

namespace rex
{
	namespace gfx
	{
		ShaderParametersStore::ShaderParametersStore(const ShaderParametersStoreDesc& desc)
			: m_param_to_location_lookup(&desc.param_map)
		{
			m_shader_parameters.reserve(desc.shader_resource_descs.size());
			for (ShaderParameterDesc shaderResourceDesc : desc.shader_resource_descs)
			{
				switch (shaderResourceDesc.type)
				{
				case ShaderParameterType::ConstantBuffer: m_shader_parameters.push_back(rsl::make_unique<ViewShaderParam>(shaderResourceDesc)); break;
				default: m_shader_parameters.push_back(rsl::make_unique<ViewTableShaderParam>(shaderResourceDesc)); break;
				}
			}
		}

		void ShaderParametersStore::set(rsl::string_view name, const ConstantBuffer* cb)
		{
			REX_ASSERT_X(m_param_to_location_lookup->contains(name), "No parameter of name \"{}\" found", name);
			ShaderParameterLocation loc = m_param_to_location_lookup->at(name);
			m_shader_parameters[loc.idx]->update_view(loc.view_offset, cb);
		}
		void ShaderParametersStore::set(rsl::string_view name, const UnorderedAccessBuffer* cb)
		{
			REX_ASSERT_X(m_param_to_location_lookup->contains(name), "No parameter of name \"{}\" found", name);
			ShaderParameterLocation loc = m_param_to_location_lookup->at(name);
			m_shader_parameters[loc.idx]->update_view(loc.view_offset, cb);
		}
		void ShaderParametersStore::set(rsl::string_view name, const Texture2D* texture)
		{
			REX_ASSERT_X(m_param_to_location_lookup->contains(name), "No parameter of name \"{}\" found", name);
			ShaderParameterLocation loc = m_param_to_location_lookup->at(name);
			m_shader_parameters[loc.idx]->update_view(loc.view_offset, texture);
		}
		void ShaderParametersStore::set(rsl::string_view name, const Sampler2D* sampler)
		{
			REX_ASSERT_X(m_param_to_location_lookup->contains(name), "No parameter of name \"{}\" found", name);
			ShaderParameterLocation loc = m_param_to_location_lookup->at(name);
			m_shader_parameters[loc.idx]->update_view(loc.view_offset, sampler);
		}
		ShaderParameterLocation ShaderParametersStore::location(rsl::string_view name) const
		{
			return m_param_to_location_lookup->at(name);
		}

		const rsl::vector<rsl::unique_ptr<ShaderParameter>>& ShaderParametersStore::params() const
		{
			return m_shader_parameters;
		}

		// Copy all parameters from another store into this store.
		// Only parameters that are named in this store are copied from the other
		void ShaderParametersStore::copy_params_from(const ShaderParametersStore* other)
		{
			for (const auto& [param_name, param_loc] : *m_param_to_location_lookup)
			{
				auto param_loc_in_other = other->m_param_to_location_lookup->at(param_name);
				const ShaderParameter* shader_param = other->m_shader_parameters[param_loc_in_other.idx].get();
				switch (shader_param->type())
				{
				case ShaderParameterType::ConstantBuffer: 
				{
					const ViewShaderParam& view_param = static_cast<const ViewShaderParam&>(*shader_param);
					m_shader_parameters[param_loc.idx] = rsl::make_unique<ViewShaderParam>(view_param); 
					break;
				}
				default: 
					const ViewTableShaderParam& view_table_param = static_cast<const ViewTableShaderParam&>(*shader_param);
					m_shader_parameters[param_loc.idx] = rsl::make_unique<ViewTableShaderParam>(view_table_param); break;
				}
			}
		}
	}
}