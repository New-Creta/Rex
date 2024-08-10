#include "rex_renderer_core/system/shader_library.h"

#include "rex_std/unordered_map.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_renderer_core/gfx/rhi.h"
#include "rex_engine/diagnostics/log.h"
#include "rex_engine/text_processing/text_processing.h"

namespace rex
{
	namespace gfx
	{
		namespace shader_lib
		{
			DEFINE_LOG_CATEGORY(LogShaderLib);

			rsl::string_view shader_name(rsl::string_view path)
			{
				return path::stem(path);
			}

			class ShaderLibrary
			{
			public:
				Shader* load(rsl::string_view path, ShaderType type)
				{
					// Check if the path exists first. Yes it's possible a shader with the same name exists
					// but to be on the extra safe side, check if the path is correct. Naming of a shader will likely
					// not be path based in the future
					if (!vfs::is_file(path))
					{
						REX_ERROR(LogShaderLib, "Failed to load shader at {} as the path does not exist", quoted(path));
						return nullptr;
					}

					rsl::string_view name = shader_name(path);

					// If shader is cached already, just return that one
					Shader* cached_shader = load(name);
					if (cached_shader)
					{
						return cached_shader;
					}

					// If shader is not cached yet, load it and cache it
					rex::memory::Blob content = vfs::read_file(path);
					rsl::unique_ptr<Shader> new_shader;
					switch (type)
					{
					case rex::gfx::ShaderType::Vertex:	new_shader = rhi::create_vertex_shader(memory::blob_to_string_view(content)); break;
					case rex::gfx::ShaderType::Pixel:		new_shader = rhi::create_pixel_shader(memory::blob_to_string_view(content));	break;
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

				Shader* load(rsl::string_view name)
				{
					auto it = m_shader_map.find(name);
					return it != m_shader_map.end()
						? it->value.get()
						: nullptr;
				}

				void clear()
				{
					m_shader_map.clear();
				}

			private:
				rsl::unordered_map<rsl::tiny_stack_string, rsl::unique_ptr<Shader>> m_shader_map;
			};

			ShaderLibrary& instance()
			{
				static ShaderLibrary instance{};
				return instance;
			}

			Shader* load(rsl::string_view path, ShaderType type)
			{
				return instance().load(path, type);
			}
			
			Shader* load(rsl::string_view name)
			{
				return instance().load(name);
			}

			void clear()
			{
				instance().clear();
			}
		}
	}
}