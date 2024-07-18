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
			Texture,
			Sampler
		};

		class Texture2D;
		class Sampler2D;

		// A material parameter is a resource entry in the material tied to a name
		class MaterialParameter
		{
		public:
			MaterialParameter(rsl::string_view name, ShaderType shaderType, s32 shaderRegister, MaterialParameterType type);

			// The name of the parameter, this matches the name of the variable in the shader it belongs to
			rsl::string_view name() const;
			// The shader register that is used for this parameter
			s32 shader_register() const;
			// The type of this parameter
			MaterialParameterType type() const;
			// The shader type this parameter belongs to
			ShaderType shader_type() const;

		private:
			rsl::string m_name;							// name of the parameter
			s32 m_shader_register;					// register of the paraneter
			ShaderType m_shader_type;				// type of shader it belongs to
			MaterialParameterType m_type;		// type if the parameter itself
		};

		// A texture material parameter is a material parameter specifically used for textures
		class TextureMaterialParameter : public MaterialParameter
		{
		public:
			TextureMaterialParameter(rsl::string_view name, ShaderType shaderType, s32 shaderRegister, Texture2D* texture = nullptr);

			// Update the internally stored texture to a new texture
			void set(Texture2D* texture);
			// Return the stored texture for this parameter
			Texture2D* texture();

		private:
			Texture2D* m_texture;
		};

		// A texture material parameter is a material parameter specifically used for samplers
		class SamplerMaterialParameter : public MaterialParameter
		{
		public:
			SamplerMaterialParameter(rsl::string_view name, ShaderType shaderType, s32 shaderRegister, Sampler2D* sampler = nullptr);

			// Update the internally stored sampler to a new sampler
			void set(Sampler2D* sampler);
			// Return the stored sampler for this parameter
			Sampler2D* sampler();

		private:
			Sampler2D* m_sampler;
		};
	}
}