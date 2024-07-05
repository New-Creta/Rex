#include "rex_renderer_core/materials/material_system.h"

#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/vfs.h"

#include "rex_engine/text_processing/text_processing.h"
#include "rex_engine/text_processing/ini_processor.h"
#include "rex_engine/diagnostics/log.h"

#include "rex_renderer_core/system/shader_pipeline.h"
#include "rex_renderer_core/gfx/rhi.h"
#include "rex_renderer_core/shader_reflection/shader_reflection.h"

#include "rex_engine/json/json.h"

namespace rex
{
	namespace gfx
	{
		DEFINE_LOG_CATEGORY(LogMaterialSystem);

		rsl::unique_ptr<Material> load_material(rsl::string_view filepath)
		{
			// Try to load the material content from disk
			if (!vfs::is_file(filepath))
			{
				REX_ERROR(LogMaterialSystem, "Cannot load material as it doesn't exists: {}", quoted(filepath));
				return nullptr;
			}

			rsl::map<rsl::string, rsl::string> map;
			map.emplace("name", "fonts_texture");
			map.emplace("type", "texture");
			map.emplace("path", "rex/textures/imgui/imgui_font.png");

			// Extract its content so that it's processable
			rex::memory::Blob file_content = vfs::read_file(filepath);
			auto json_blob = json::parse(file_content);
			
			if (json_blob.is_discarded())
			{
				REX_WARN(LogMaterialSystem, "Failed to process material at {}.", filepath);
				return nullptr;
			}

			rsl::string_view vertex_shader = json_blob["vertex_shader"].get<rsl::string_view>();
			rsl::string_view pixel_shader = json_blob["pixel_shader"].get<rsl::string_view>();

			// Process material content so we can create a material object out of it
			ShaderPipeline shader_pipeline{};
			shader_pipeline.vs = rhi::create_vertex_shader(vertex_shader);
			shader_pipeline.ps = rhi::create_pixel_shader(pixel_shader);

			// Based on the shader pipeline reflection, we can initialize the shader parameters
			ShaderPipelineReflection shader_pipeline_reflection = reflect_shader_pipeline(shader_pipeline);

			const auto& parameters_array = json_blob["parameters"];
			for (const auto& param : parameters_array)
			{
				rsl::string_view type = param["type"];
				constexpr rsl::string_view s = "spo";
				constexpr auto res = rsl::hash<rsl::string_view>{}(s);
				

				rsl::string_view name = param["name"];
				rsl::string_view path = param["path"];
			}

			// Create the material object and return it
			return rhi::create_material(rsl::move(shader_pipeline), rsl::move(shader_pipeline_reflection));
		}
	}
}