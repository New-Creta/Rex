#pragma once

#include "rex_std/string.h"
#include "rex_std/string_view.h"

#include "rex_engine/engine/types.h"

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

		class MaterialParameter
		{
		public:
			MaterialParameter(rsl::string_view name, s32 shaderRegister, MaterialParameterType type);

			rsl::string_view name() const;
			s32 shader_register() const;
			MaterialParameterType type() const;

		private:
			rsl::string m_name;
			s32 m_shader_register;
			MaterialParameterType m_type;
		};

		class TextureMaterialParameter : public MaterialParameter
		{
		public:
			TextureMaterialParameter(rsl::string_view name, s32 shaderRegister, Texture2D* texture = nullptr);

			void set(Texture2D* texture);
			Texture2D* texture();

		private:
			Texture2D* m_texture;
		};

		class SamplerMaterialParameter : public MaterialParameter
		{
		public:
			SamplerMaterialParameter(rsl::string_view name, s32 shaderRegister, Sampler2D* sampler = nullptr);

			void set(Sampler2D* sampler);
			Sampler2D* sampler();

		private:
			Sampler2D* m_sampler;
		};
	}
}