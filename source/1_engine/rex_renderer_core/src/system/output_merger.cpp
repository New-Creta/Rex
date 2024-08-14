#include "rex_renderer_core/system/output_merger.h"

#include "rex_std/bonus/utility.h"

namespace rex
{
	namespace gfx
	{
		BlendDesc load_blend_state(const rex::json::json& blendState)
		{
			BlendDesc desc{};

			desc.enable_alpha_to_coverage = blendState["enable_alpha_to_coverage"];
			desc.independent_blend_state = blendState["independent_blend_state"];
			const rsl::vector<rex::json::json>& render_targets = blendState["render_targets"];

			for (s32 i = 0; i < render_targets.size(); ++i)
			{
				desc.render_target[i].blend_enable = render_targets[i]["blend_enable"];
				desc.render_target[i].src_blend = rsl::enum_refl::enum_cast<Blend>(rsl::string_view(render_targets[i]["src_blend"])).value();
				desc.render_target[i].dst_blend = rsl::enum_refl::enum_cast<Blend>(rsl::string_view(render_targets[i]["dst_blend"])).value();
				desc.render_target[i].blend_op = rsl::enum_refl::enum_cast<BlendOp>(rsl::string_view(render_targets[i]["blend_op"])).value();
				desc.render_target[i].src_blend_alpha = rsl::enum_refl::enum_cast<Blend>(rsl::string_view(render_targets[i]["src_blend_alpha"])).value();
				desc.render_target[i].dst_blend_alpha = rsl::enum_refl::enum_cast<Blend>(rsl::string_view(render_targets[i]["dst_blend_alpha"])).value();
				desc.render_target[i].blend_op_alpha = rsl::enum_refl::enum_cast<BlendOp>(rsl::string_view(render_targets[i]["blend_op_alpha"])).value();
				desc.render_target[i].render_target_write_mask = rsl::enum_refl::enum_cast<RenderTargetWriteMask>(rsl::string_view(render_targets[i]["render_target_write_mask"])).value();
			}

			return desc;
		}

		RasterStateDesc load_raster_state(const json::json& rasterState)
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

		DepthStencilDesc load_depth_stencil(const json::json& depthStencil)
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

		OutputMergerDesc load_output_merger_from_json(const json::json& jsonBlob)
		{
			OutputMergerDesc output_merger_desc{};
			if (jsonBlob.contains("blend_state"))
			{
				output_merger_desc.blend_state = load_blend_state(jsonBlob["blend_state"]);
			}

			if (jsonBlob.contains("raster_state"))
			{
				output_merger_desc.raster_state = load_raster_state(jsonBlob["raster_state"]);
			}

			if (jsonBlob.contains("depth_stencil"))
			{
				output_merger_desc.depth_stencil_state = load_depth_stencil(jsonBlob["depth_stencil"]);
			}

			return output_merger_desc;
		}
	}
}