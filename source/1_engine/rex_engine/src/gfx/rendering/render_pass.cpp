#include "rex_engine/gfx/rendering/render_pass.h"

#include "rex_engine/gfx/system/rhi.h"
#include "rex_engine/gfx/shader_reflection/shader_reflection_cache.h"

#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/text_processing/text_processing.h"
#include "rex_engine/diagnostics/log.h"
#include "rex_engine/gfx/system/shader_library.h"
#include "rex_engine/gfx/rendering/renderer.h"

#include "rex_engine/gfx/system/gpu_engine.h"
#include "rex_std/algorithm.h"

namespace rex
{
	namespace gfx
	{
		DEFINE_LOG_CATEGORY(LogRenderPass);

		RenderPass::RenderPass(const RenderPassDesc& desc)
			: m_name(desc.name)
		{
			m_pso_desc = desc.pso_desc;
			m_pso = rhi::create_pso(desc.pso_desc);
			const ShaderPipelineReflection& reflection = shader_reflection_cache::load(desc.pso_desc.shader_pipeline);
			m_parameters_store = rsl::make_unique<ShaderParametersStore>(reflection.renderpass_param_store_desc);
			m_framebuffer = rsl::make_unique<FrameBuffer>(desc.framebuffer_desc);
		}

		void RenderPass::bind_to(RenderContext* ctx)
		{
			ctx->set_pipeline_state(m_pso.get());
			ctx->set_root_signature(m_pso->root_signature());
			ctx->set_primitive_topology(PrimitiveTopology::TriangleList);
			ctx->set_blend_factor(m_blend_factor);

			m_framebuffer->bind_to(ctx);
			bind_params_to_pipeline(m_parameters_store.get(), ctx);
		}

		void RenderPass::bind_material(RenderContext* ctx, Material* material)
		{
			MaterialPsoOverwrite pso_overwrite_result = material->load_pso_overwrite(m_pso_desc);

			if (pso_overwrite_result.is_overwritten)
			{
				if (!m_material_to_pipeline_state.contains(material))
				{
					// It's possible the slots have change if a new PSO was bound
					// So we need calcualte the new slot positions.
					MaterialPipelineState mat_pipeline_state{};
					const ShaderPipelineReflection& reflection = shader_reflection_cache::load(pso_overwrite_result.shader_pipeline);
					mat_pipeline_state.parameter_store = rsl::make_unique<ShaderParametersStore>(reflection.renderpass_param_store_desc);

					// Copy the render pass parameters into the new slot positions
					mat_pipeline_state.parameter_store->copy_params_from(m_parameters_store.get());
					m_material_to_pipeline_state.emplace(material, rsl::move(mat_pipeline_state));
				}

				// Bind the render pass parameters into the new store used for this material
				bind_params_to_pipeline(m_material_to_pipeline_state.at(material).parameter_store.get(), ctx);
			}

			// Now bind the material to the context
			material->bind_to(ctx);
		}

		void RenderPass::set(rsl::string_view name, const ConstantBuffer* constantBuffer)
		{
			m_parameters_store->set(name, constantBuffer);
		}
		void RenderPass::set(rsl::string_view name, const Texture2D* texture)
		{
			m_parameters_store->set(name, texture);
		}
		void RenderPass::set(rsl::string_view name, const Sampler2D* sampler)
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

		void RenderPass::sort_draw_lists(rsl::vector<DrawList>& drawList)
		{
			// First split the drawlist up in the group that uses the same shader pipeline as the render pass
			// and a group that doesn't
			auto it = rsl::partition(drawList.begin(), drawList.end(),
				[this](const DrawList& drawlist)
				{
					return drawlist.material->uses_shader_pipeline(m_pso_desc.shader_pipeline);
				});
			
			// for the group that doesn't, sort them based on the material to minimize PSO swaps later on
			rsl::sort(it, drawList.end(),
				[this](const DrawList& lhs, const DrawList& rhs)
				{
					return lhs.material < rhs.material;
				});
		}

		void RenderPass::bind_params_to_pipeline(ShaderParametersStore* paramsStore, RenderContext* ctx)
		{
			const rsl::vector<rsl::unique_ptr<ShaderParameter>>& shader_params = paramsStore->params();
			for (const auto& param : shader_params)
			{
				param->bind_to(ctx);
			}
		}

	}
}