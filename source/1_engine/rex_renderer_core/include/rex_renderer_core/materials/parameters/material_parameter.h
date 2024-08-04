#pragma once

#include "rex_std/string.h"
#include "rex_std/string_view.h"

#include "rex_engine/engine/types.h"

#include "rex_renderer_core/resources/texture_2d.h"
#include "rex_renderer_core/resources/sampler_2d.h"

#include "rex_renderer_core/system/shader_type.h"

namespace rex
{
	namespace gfx
	{
		enum class MaterialParameterType
		{
			Undefined,
			Texture,
			Sampler
		};

		class Texture2D;
		class Sampler2D;
		class Resource;

		// A material parameter is a resource entry in the material tied to a name
		class MaterialParameter
		{
		public:
			MaterialParameter(rsl::string_view name, ShaderType shaderType, s32 shaderRegister, MaterialParameterType type);
			virtual ~MaterialParameter() = default;

			// The name of the parameter, this matches the name of the variable in the shader it belongs to
			rsl::string_view name() const;
			// The shader register that is used for this parameter
			s32 shader_register() const;
			// The type of this parameter
			MaterialParameterType type() const;
			// The shader type this parameter belongs to
			ShaderType shader_type() const;
			// Return the wrapped resource object
			Resource* resource();

			void set_resource(Resource* resource);

		private:
			rsl::string m_name;							// name of the parameter
			s32 m_shader_register;					// register of the paraneter
			ShaderType m_shader_type;				// type of shader it belongs to
			MaterialParameterType m_type;		// type if the parameter itself
			Resource* m_resource;
		};
	}
}