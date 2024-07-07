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

#include "rex_engine/string/stringid.h"

namespace rex
{
	namespace gfx
	{
		DEFINE_LOG_CATEGORY(LogMaterialSystem);

		void init_material_parameters(Material* material, const json::json& parametersBlob)
		{
			for (const auto& param : parametersBlob)
			{
				rsl::string_view type = param["type"];
				rsl::hash_result type_hash = rsl::comp_hash(type);

				MaterialParameterType param_type;
				switch (type_hash)
				{
				case "texture"_sid: param_type = MaterialParameterType::Texture; break;
				case "sampler"_sid: param_type = MaterialParameterType::Sampler; break;
				default: REX_ASSERT("Invalid material param type {}", quoted(type)); break;
				}

				rsl::string_view name = param["name"];
				rsl::string_view path = param["path"];

				// Currently won't work as the resources created should not return unique ptrs as its lifetime should be managed elsewhere
				// However, to get around the compiler errors, we implement it as this for now
				switch (param_type)
				{
				case rex::gfx::MaterialParameterType::Texture: material->set_texture(name, rhi::create_texture2d(path).get());
				case rex::gfx::MaterialParameterType::Sampler: material->set_sampler(name, rhi::create_sampler2d(path).get());
				}
			}
		}

		rsl::unique_ptr<Material> load_material(rsl::string_view filepath)
		{
			REX_VERBOSE(LogMaterialSystem, "Loading material {}", quoted(filepath));

			// Try to load the material content from disk
			if (!vfs::is_file(filepath))
			{
				REX_ERROR(LogMaterialSystem, "Cannot load material as it doesn't exists: {}", quoted(filepath));
				return nullptr;
			}

			// Extract its content so that it's processable
			rex::memory::Blob file_content = vfs::read_file(filepath);
			auto json_blob = json::parse(file_content);
			
			if (json_blob.is_discarded())
			{
				REX_WARN(LogMaterialSystem, "Failed to process material at {}.", filepath);
				return nullptr;
			}

			// A vertex and pixel shader is mandatory for each material
			REX_ASSERT_X(json_blob.contains("vertex_shader"), "No vertex shader found in material, this is not allowed. Path: {}", quoted(filepath));
			REX_ASSERT_X(json_blob.contains("pixel_shader"), "No pixel shader found in material, this is not allowed. Path: {}", quoted(filepath));

			rsl::string_view vertex_shader = json_blob["vertex_shader"].get<rsl::string_view>();
			rsl::string_view pixel_shader = json_blob["pixel_shader"].get<rsl::string_view>();

			// Process material content so we can create a material object out of it
			ShaderPipeline shader_pipeline{};
			shader_pipeline.vs = rhi::create_vertex_shader(vertex_shader);
			shader_pipeline.ps = rhi::create_pixel_shader(pixel_shader);

			// Create the material object
			rsl::unique_ptr<Material> material = rhi::create_material(rsl::move(shader_pipeline));

			// Load in the parameters values from the material
			init_material_parameters(material.get(), json_blob["parameters"]);
		}
	}
}