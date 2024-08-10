#include "rex_renderer_core/rendering/render_pass.h"

#include "rex_renderer_core/gfx/rhi.h"
#include "rex_renderer_core/shader_reflection/shader_reflection_cache.h"

#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/text_processing/text_processing.h"
#include "rex_engine/diagnostics/log.h"
#include "rex_renderer_core/system/shader_library.h"

namespace rex
{
	namespace gfx
	{
		DEFINE_LOG_CATEGORY(LogRenderPass);

		rsl::unique_ptr<RenderPass> load_from_json_path(rsl::string_view filepath)
		{
			// Try to load the render pass content from disk
			if (!vfs::is_file(filepath))
			{
				REX_ERROR(LogRenderPass, "Cannot load render pass as it doesn't exists: {}", quoted(filepath));
				return nullptr;
			}

			// Extract its content so that it's processable
			// Materials are saved in json format, so we use a json parser
			rex::memory::Blob file_content = vfs::read_file(filepath);
			auto json_blob = rex::json::parse(file_content);

			// If the json blob is discarded, that means an error occurred during json parsing
			if (json_blob.is_discarded())
			{
				REX_WARN(LogRenderPass, "Failed to process render pass at {}.", filepath);
				return nullptr;
			}

			// A vertex and pixel shader is mandatory for each render pass
			REX_ASSERT_X(json_blob.contains("vertex_shader"), "No vertex shader found in render pass, this is not allowed. Path: {}", quoted(filepath));
			REX_ASSERT_X(json_blob.contains("pixel_shader"), "No pixel shader found in render pass, this is not allowed. Path: {}", quoted(filepath));

			rsl::string_view vertex_shader = json_blob["vertex_shader"].get<rsl::string_view>();
			rsl::string_view pixel_shader = json_blob["pixel_shader"].get<rsl::string_view>();

			// Process render pass content so we can create a render pass object out of it
			RenderPassDesc pass_desc{};
			pass_desc.pso_desc.shader_pipeline.vs = shader_lib::load(vertex_shader, ShaderType::Vertex);
			pass_desc.pso_desc.shader_pipeline.ps = shader_lib::load(pixel_shader, ShaderType::Pixel);

			// Load additional settings for the render pass's initialization if they're available
			pass_desc.pso_desc.output_merger = load_output_merger_from_json(json_blob["output_merger"]);
			pass_desc.pso_desc.input_layout = load_input_layout_from_json(json_blob["input_layout"]);

			return rsl::make_unique<RenderPass>(pass_desc);
		}

		RenderPass::RenderPass(const RenderPassDesc& desc)
			: m_name(desc.name)
		{
			m_pso = rhi::create_pso(desc.pso_desc);
			ShaderPipelineReflection& reflection = shader_reflection_cache::load(desc.pso_desc.shader_pipeline);
			m_parameters_store = rsl::make_unique<ShaderParametersStore>(reflection.renderpass_param_store_desc);
		}

		void RenderPass::bind_to(RenderContext* ctx)
		{
			ctx->set_pipeline_state(m_pso.get());
			ctx->set_root_signature(m_pso->root_signature());
			ctx->set_primitive_topology(PrimitiveTopology::TriangleList);
			ctx->set_blend_factor(m_blend_factor);

			const rsl::vector<rsl::unique_ptr<ShaderParameter>>& shader_params = m_parameters_store->params();
			for (const auto& shader_resource : shader_params)
			{
				shader_resource->bind_to(ctx);
			}
		}

		void RenderPass::set(rsl::string_view name, ConstantBuffer* constantBuffer)
		{
			m_parameters_store->set(name, constantBuffer);
		}
		void RenderPass::set(rsl::string_view name, Texture2D* texture)
		{
			m_parameters_store->set(name, texture);
		}
		void RenderPass::set(rsl::string_view name, Sampler2D* sampler)
		{
			m_parameters_store->set(name, sampler);
		}
		s32 RenderPass::slot(rsl::string_view name) const
		{
			return m_parameters_store->location(name).slot;
		}

		void RenderPass::set_blend_factor(const BlendFactor& blendFactor)
		{
			m_blend_factor = blendFactor;
		}

	}
}