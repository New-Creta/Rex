#pragma once

#include "rex_engine/engine/globals.h"
#include "rex_engine/gfx/resources/shader.h"
#include "rex_engine/gfx/system/shader_type.h"

#include "rex_std/unordered_map.h"
#include "rex_std/memory.h"
#include "rex_std/string_view.h"
#include "rex_std/bonus/string.h"

namespace rex
{
	namespace gfx
	{
		class ShaderLibrary
		{
		public:
			// Load a shader from the cache, given a filepath and shader type
			Shader* load(rsl::string_view path, ShaderType type);
			// Load a shader from the cache, given a shader name
			Shader* load(rsl::string_view name);

			// Clear the cache
			void clear();

			scratch_string shader_name(rsl::string_view path, ShaderType type) const;

		private:
			rsl::unordered_map<rsl::tiny_stack_string, rsl::unique_ptr<Shader>> m_shader_map;
		};

		namespace shader_lib
		{
			void init(globals::GlobalUniquePtr<ShaderLibrary> shaderLib);
			ShaderLibrary* instance();
			void shutdown();
		}
	}
}