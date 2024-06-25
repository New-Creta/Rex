#include "rex_renderer_core/materials/material_system.h"

#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/vfs.h"

#include "rex_engine/text_processing/text_processing.h"
#include "rex_engine/text_processing/ini_processor.h"
#include "rex_engine/diagnostics/log.h"

#include "rex_renderer_core/system/shader_pipeline.h"
#include "rex_renderer_core/gfx/rhi.h"
#include "rex_renderer_core/shader_reflection/shader_reflection.h"

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

			// Extract its content so that it's processable
			rex::memory::Blob file_content = vfs::read_file(filepath);
			IniProcessor ini_processor(file_content);
			Error err = ini_processor.process();
			if (err)
			{
				REX_WARN(LogMaterialSystem, "Failed to process material at {}. Error: {}", filepath, err.error_msg());
				return nullptr;
			}

			rsl::string_view vertex_shader = ini_processor.get("vertex_shader");
			rsl::string_view pixel_shader = ini_processor.get("pixel_shader");

			// Process material content so we can create a material object out of it
			ShaderPipeline shader_pipeline{};
			shader_pipeline.vs = rhi::create_vertex_shader(vertex_shader);
			shader_pipeline.ps = rhi::create_pixel_shader(pixel_shader);

			ShaderPipelineReflection shader_pipeline_reflection = reflect_shader_pipeline(shader_pipeline);

			// Create the material object and return it
			return rhi::create_material(rsl::move(shader_pipeline), rsl::move(shader_pipeline_reflection));
		}
	}
}