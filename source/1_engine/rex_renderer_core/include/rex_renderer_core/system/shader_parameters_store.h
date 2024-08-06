#pragma once

#include "rex_std/vector.h"
#include "rex_std/unordered_map.h"

#include "rex_renderer_core/system/shader_type.h"
#include "rex_renderer_core/shader_reflection/shader_pipeline_reflection.h"
#include "rex_renderer_core/resources/resource.h"
#include "rex_renderer_core/system/shader_parameter_location.h"

namespace rex
{
	namespace gfx
	{
		enum class ShaderParameterType2
		{
			ConstantBuffer,
			Texture,
			Sampler
		};

		class ConstantBuffer;
		class Texture2D;
		class Sampler2D;

		class ShaderParameter
		{
		public:
			ShaderParameter(ShaderParameterType2 type, ShaderType owningShaderType);

			void set_resource(Resource* resource);
			Resource* resource();
      ShaderType owning_shader_type() const;

    private:
      Resource* m_resource;
      ShaderType m_owning_shader_type;
			ShaderParameterType2 m_type;
		};

		struct ShaderParametersStoreDesc
		{
			// This stores the location of a certain parameter inside the entire pipeline's parameter list
			rsl::unordered_map<rsl::string, ShaderParameterLocation> param_map;

			// This stores the number of views per range.
			// The number of ranges is implied by the size of this vector
			rsl::vector<ShaderResourceDesc> shader_resource_descs;
		};

		class ShaderParametersStore
		{
		public:
			ShaderParametersStore(const ShaderParametersStoreDesc& desc);

			void set(rsl::string_view name, ConstantBuffer* cb);
			void set(rsl::string_view name, Texture2D* texture);
			void set(rsl::string_view name, Sampler2D* sampler);

			const rsl::vector<ShaderResource>& params() const;

		private:
			// To make parameters easily accessible to users, we have a map from name of parameter to their view idx
			// This view idx is the index within the parameter container that the param type belongs to
			const rsl::unordered_map<rsl::string, ShaderParameterLocation>* m_param_to_location_lookup;

			// Holds the list of resources that are directly tied to the shader
			rsl::vector<ShaderResource> m_shader_resources;
		};
	}
}