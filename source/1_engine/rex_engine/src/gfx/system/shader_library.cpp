#include "rex_engine/gfx/system/shader_library.h"

#include "rex_std/unordered_map.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/gfx/graphics.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/text_processing/text_processing.h"

namespace rex
{
	namespace gfx
	{
		DEFINE_LOG_CATEGORY(LogShaderLib);

		Shader* ShaderLibrary::load(rsl::string_view path, ShaderType type)
		{
			// Check if the path exists first. Yes it's possible a shader with the same name exists
			// but to be on the extra safe side, check if the path is correct. Naming of a shader will likely
			// not be path based in the future
			if (!vfs::instance()->exists(path))
			{
				REX_ERROR(LogShaderLib, "Failed to load shader at {} as the path does not exist", quoted(path));
				return nullptr;
			}

			scratch_string name = shader_name(path, type);

			// If shader is cached already, just return that one
			Shader* cached_shader = load(name);
			if (cached_shader)
			{
				return cached_shader;
			}

			// If shader is not cached yet, load it and cache it
			rex::memory::Blob content = vfs::instance()->read_file(path);
			rsl::unique_ptr<Shader> new_shader;
			switch (type)
			{
			case rex::gfx::ShaderType::Vertex:	new_shader = gfx::gal::instance()->create_vertex_shader(memory::blob_to_string_view(content)); break;
			case rex::gfx::ShaderType::Pixel:		new_shader = gfx::gal::instance()->create_pixel_shader(memory::blob_to_string_view(content));	break;
			default: break;
			}

			if (!new_shader)
			{
				REX_ERROR(LogShaderLib, "Failed to load shader at {}.", quoted(path));
				return nullptr;
			}

			auto res = m_shader_map.emplace(name, rsl::move(new_shader));
			return res.inserted_element->value.get();
		}

		Shader* ShaderLibrary::load(rsl::string_view name)
		{
			auto it = m_shader_map.find(name);
			return it != m_shader_map.end()
				? it->value.get()
				: nullptr;
		}

		void ShaderLibrary::clear()
		{
			m_shader_map.clear();
		}

		scratch_string ShaderLibrary::shader_name(rsl::string_view path, ShaderType type) const
		{
			return scratch_string(rsl::format("{}_{}", path::stem(path), rsl::enum_refl::enum_name(type)));
		}

		namespace shader_lib
		{
			globals::GlobalUniquePtr<ShaderLibrary> g_shader_lib;
			void init(globals::GlobalUniquePtr<ShaderLibrary> shaderLib)
			{
				g_shader_lib = rsl::move(shaderLib);
			}
			ShaderLibrary* instance()
			{
				return g_shader_lib.get();
			}
			void shutdown()
			{
				g_shader_lib.reset();
			}
		}
	}
}