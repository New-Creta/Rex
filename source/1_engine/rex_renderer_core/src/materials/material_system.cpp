#include "rex_renderer_core/materials/material_system.h"

#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/vfs.h"

#include "rex_engine/text_processing/text_processing.h"
#include "rex_engine/text_processing/ini_processor.h"
#include "rex_engine/diagnostics/log.h"

#include "rex_renderer_core/system/shader_pipeline.h"
#include "rex_renderer_core/gfx/rhi.h"
#include "rex_renderer_core/shader_reflection/shader_signature.h"

#include "rex_engine/string/stringid.h"
#include "rex_engine/json/json.h"

#include "rex_std/string.h"

namespace rex
{
	namespace gfx
	{
		DEFINE_LOG_CATEGORY(LogMaterialSystem);
		using json = nlohmann::json;

		BlendDesc load_blend_state(const json& blendState)
		{
			BlendDesc desc{};

			desc.enable_alpha_to_coverage = blendState["enable_alpha_to_coverage"];
			desc.independent_blend_state = blendState["independent_blend_state"];
			const rsl::vector<json>& render_targets = blendState["render_targets"];

			for (s32 i = 0; i < render_targets.size(); ++i)
			{
				desc.render_target[i].blend_enable						  = render_targets[i]["blend_enable"];
				desc.render_target[i].src_blend									= rsl::enum_refl::enum_cast<Blend>(rsl::string_view(render_targets[i]["src_blend"])).value();
				desc.render_target[i].dst_blend									= rsl::enum_refl::enum_cast<Blend>(rsl::string_view(render_targets[i]["dst_blend"])).value();
				desc.render_target[i].blend_op									= rsl::enum_refl::enum_cast<BlendOp>(rsl::string_view(render_targets[i]["blend_op"])).value();
				desc.render_target[i].src_blend_alpha					  = rsl::enum_refl::enum_cast<Blend>(rsl::string_view(render_targets[i]["src_blend_alpha"])).value();
				desc.render_target[i].dst_blend_alpha					  = rsl::enum_refl::enum_cast<Blend>(rsl::string_view(render_targets[i]["dst_blend_alpha"])).value();
				desc.render_target[i].blend_op_alpha						= rsl::enum_refl::enum_cast<BlendOp>(rsl::string_view(render_targets[i]["blend_op_alpha"])).value();
				desc.render_target[i].render_target_write_mask  = rsl::enum_refl::enum_cast<RenderTargetWriteMask>(rsl::string_view(render_targets[i]["render_target_write_mask"])).value();
			}

			return desc;
		}

		RasterStateDesc load_raster_state(const json& rasterState)
		{
			RasterStateDesc desc{};

			desc.fill_mode = rsl::enum_refl::enum_cast<FillMode>(rsl::string_view(rasterState["fill_mode"])).value();
			desc.cull_mode = rsl::enum_refl::enum_cast<CullMode>(rsl::string_view(rasterState["cull_mode"])).value();
			desc.front_ccw = rasterState["front_ccw"];
			desc.depth_bias = rasterState["depth_bias"];
			desc.depth_bias_clamp = rasterState["depth_bias_clamp"];
			desc.sloped_scale_depth_bias = rasterState["sloped_scale_depth_bias"];
			desc.depth_clip_enable = rasterState["depth_clip_enable"];
			desc.multisample_enable = rasterState["multisample_enable"];
			desc.aa_lines_enable = rasterState["aa_lines_enable"];
			desc.forced_sample_count = rasterState["forced_sample_count"];

			return desc;
		}

		DepthStencilDesc load_depth_stencil(const json& depthStencil)
		{
			DepthStencilDesc desc{};

			desc.depth_enable = depthStencil["depth_enable"];
			desc.depth_write_mask = rsl::enum_refl::enum_cast<DepthWriteMask>(rsl::string_view(depthStencil["depth_write_mask"])).value();
			desc.depth_func = rsl::enum_refl::enum_cast<ComparisonFunc>(rsl::string_view(depthStencil["depth_func"])).value();
			desc.stencil_enable = depthStencil["stencil_enable"];

			desc.front_face.stencil_depth_fail_op = rsl::enum_refl::enum_cast<StencilOp>(rsl::string_view(depthStencil["front_face"]["depth_fail_op"])).value();
			desc.front_face.stencil_fail_op = rsl::enum_refl::enum_cast<StencilOp>(rsl::string_view(depthStencil["front_face"]["stencil_fail_op"])).value();
			desc.front_face.stencil_func = rsl::enum_refl::enum_cast<ComparisonFunc>(rsl::string_view(depthStencil["front_face"]["stencil_func"])).value();
			desc.front_face.stencil_pass_op = rsl::enum_refl::enum_cast<StencilOp>(rsl::string_view(depthStencil["front_face"]["stencil_pass_op"])).value();

			desc.back_face.stencil_depth_fail_op = rsl::enum_refl::enum_cast<StencilOp>(rsl::string_view(depthStencil["back_face"]["depth_fail_op"])).value();
			desc.back_face.stencil_fail_op = rsl::enum_refl::enum_cast<StencilOp>(rsl::string_view(depthStencil["back_face"]["stencil_fail_op"])).value();
			desc.back_face.stencil_func = rsl::enum_refl::enum_cast<ComparisonFunc>(rsl::string_view(depthStencil["back_face"]["stencil_func"])).value();
			desc.back_face.stencil_pass_op = rsl::enum_refl::enum_cast<StencilOp>(rsl::string_view(depthStencil["back_face"]["stencil_pass_op"])).value();


			return desc;
		}

		MaterialConstructSettings load_mat_construct_settings_from_json(const json& jsonBlob)
		{
			MaterialConstructSettings mat_construct_settings{};
			if (jsonBlob.contains("blend_state"))
			{
				mat_construct_settings.blend = load_blend_state(jsonBlob["blend_state"]);
			}

			if (jsonBlob.contains("raster_state"))
			{
				mat_construct_settings.raster_state = load_raster_state(jsonBlob["raster_state"]);
			}

			if (jsonBlob.contains("depth_stencil"))
			{
				mat_construct_settings.depth_stencil = load_depth_stencil(jsonBlob["depth_stencil"]);
			}

			return mat_construct_settings;
		}

		void init_material_parameters(Material* /*material*/, const json& parametersBlob)
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
				case rex::gfx::MaterialParameterType::Texture: break; // material->set_texture(name, rhi::create_texture2d(path).get());
				case rex::gfx::MaterialParameterType::Sampler: break; // material->set_sampler(name, rhi::create_sampler2d(path).get());
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
			// Materials are saved in json format, so we use a json parser
			rex::memory::Blob file_content = vfs::read_file(filepath);
			auto json_blob = rex::json::parse(file_content);
			
			// If the json blob is discarded, that means an error occurred during json parsing
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
			shader_pipeline.vs = rhi::create_vertex_shader(memory::blob_to_string_view(vfs::read_file(vertex_shader)));
			shader_pipeline.ps = rhi::create_pixel_shader(memory::blob_to_string_view(vfs::read_file(pixel_shader)));

			// Load additional settings for the material's initialization if they're available
			MaterialConstructSettings mat_construct_settings = load_mat_construct_settings_from_json(json_blob);

			// Create the material object
			rsl::unique_ptr<Material> material = rhi::create_material(rsl::move(shader_pipeline), mat_construct_settings);

			// Load in the parameters values from the material
			init_material_parameters(material.get(), json_blob["parameters"]); // infinite loop here when inserting into json

			return material;
		}
	}
}