#pragma once

#include "rex_std/vector.h"
#include "rex_std/unordered_map.h"

#include "rex_renderer_core/system/shader_type.h"
#include "rex_renderer_core/resources/resource.h"
#include "rex_renderer_core/system/shader_parameter_location.h"
#include "rex_renderer_core/system/shader_parameter.h"

namespace rex
{
	namespace gfx
	{
		class ConstantBuffer;
		class Texture2D;
		class Sampler2D;

		struct ShaderParametersStoreDesc
		{
			// This stores the location of a certain parameter inside the entire pipeline's parameter list
			rsl::unordered_map<rsl::string, ShaderParameterLocation> param_map;

			// This stores the number of views per range.
			// The number of ranges is implied by the size of this vector
			rsl::vector<ShaderParameterDesc> shader_resource_descs;
		};

		class ShaderParametersStore
		{
		public:
			ShaderParametersStore(const ShaderParametersStoreDesc& desc);

			void set(rsl::string_view name, ConstantBuffer* cb);
			void set(rsl::string_view name, Texture2D* texture);
			void set(rsl::string_view name, Sampler2D* sampler);

			ShaderParameterLocation location(rsl::string_view name) const;

			const rsl::vector<rsl::unique_ptr<ShaderParameter>>& params() const;

		private:
			// To make parameters easily accessible to users, we have a map from name of parameter to their view idx
			// This view idx is the index within the parameter container that the param type belongs to
			const rsl::unordered_map<rsl::string, ShaderParameterLocation>* m_param_to_location_lookup;

			// Holds the list of resources that are directly tied to the shader
			rsl::vector<rsl::unique_ptr<ShaderParameter>> m_shader_resources;
		};
	}
}