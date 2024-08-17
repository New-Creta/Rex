#include "rex_renderer_core/rendering/scene_renderer.h"

#include "rex_renderer_core/resources/constant_buffer.h"
#include "rex_renderer_core/gfx/rhi.h"

#include "rex_renderer_core/rendering/camera.h"

#include "rex_renderer_core/gfx/copy_context.h"
#include "rex_renderer_core/gfx/graphics.h"

#include "rex_engine/filesystem/vfs.h"

#include "rex_renderer_core/system/shader_library.h"
#include "rex_renderer_core/scenegraph/scene.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_renderer_core/system/gpu_engine.h"

namespace rex
{
	namespace gfx
	{
		DEFINE_LOG_CATEGORY(LogSceneRenderer);

		SceneRenderer::SceneRenderer()
			: m_scene_data()
		{
			init_gpu_resources();
			init_render_passes();
		}

		void SceneRenderer::new_frame()
		{
			// Nothing to implement
		}

		void SceneRenderer::update_scene_data(const SceneData& sceneData)
		{
			m_scene_data = sceneData;
		}

		void SceneRenderer::render()
		{
			upload_scene_data();

			m_scene_data.scene->submit_geo_to_renderer(this);

			flush_draw_lists();
		}

		// flush the drawlist to the gpu
		void SceneRenderer::flush_draw_lists()
		{
			auto current_ctx = new_render_ctx();
			f32 viewport_width = static_cast<f32>(m_scene_data.viewport_width);
			f32 viewport_height = static_cast<f32>(m_scene_data.viewport_height);
			Viewport viewport = { 0.0f, 0.0f, viewport_width, viewport_height, 0.0f, 1.0f };
			current_ctx->set_viewport(viewport);

			ScissorRect rect = { 0, 0, viewport_width, viewport_height };
			current_ctx->set_scissor_rect(rect);

			geometry_pass(current_ctx.get());
		}

		void SceneRenderer::init_gpu_resources()
		{
			m_cb_view_data = rhi::create_constant_buffer(sizeof(PerViewData), "Per View Data");
			m_cb_scene_data = rhi::create_constant_buffer(sizeof(PerSceneData), "Per Scene Data");

			// The number of objects per scene currently has a hard limit
			// If we hit this limit we can upscale it or come up with a different strategy
			// of how to store per instance data for objects and uploading it to the GPU
			// Perhaps a ring buffer will suffice here
			m_per_instance_cbs.reserve(s_max_number_of_objects_per_scene);
			rsl::fmt_stack_string debugName;
			for (s32 i = 0; i < s_max_number_of_objects_per_scene; ++i)
			{
				debugName = rsl::format("Per Instance Data {}", i);
				m_per_instance_cbs.push_back(rhi::create_constant_buffer(sizeof(PerInstanceData), debugName));
			}
		}

		void SceneRenderer::init_render_passes()
		{
			init_geometry_render_pass();
		}

		void SceneRenderer::init_geometry_render_pass()
		{
			RenderPassDesc geo_pass_desc{};
			geo_pass_desc.name = "Geometry Pass";

			// Pipeline State

			// Define the depth stencil settings
			geo_pass_desc.pso_desc.output_merger.depth_stencil_state.depth_enable = true;
			geo_pass_desc.pso_desc.output_merger.depth_stencil_state.depth_write_mask = DepthWriteMask::DepthWriteMaskAll;
			geo_pass_desc.pso_desc.output_merger.depth_stencil_state.depth_func = ComparisonFunc::Less;

			// Define the input layout
			geo_pass_desc.pso_desc.input_layout = 
			{
				{ ShaderSemantic::Position, ShaderArithmeticType::Float3    },
				{ ShaderSemantic::Normal,   ShaderArithmeticType::Float3    },
				{ ShaderSemantic::TexCoord, ShaderArithmeticType::Float2    } 
			};

			// Define the shaders
			geo_pass_desc.pso_desc.shader_pipeline.vs = shader_lib::load(vfs::abs_path(MountingPoint::EngineShaders, "basic_vertex.hlsl"), ShaderType::Vertex);
			geo_pass_desc.pso_desc.shader_pipeline.ps = shader_lib::load(vfs::abs_path(MountingPoint::EngineShaders, "basic_pixel.hlsl"), ShaderType::Pixel);

			// Define the primitive topology
			geo_pass_desc.pso_desc.primitive_topology = PrimitiveTopologyType::Triangle;

			// Define the framebuffer attachments
			geo_pass_desc.framebuffer_desc.emplace_back(swapchain_frame_buffer_handle());
			geo_pass_desc.framebuffer_desc.emplace_back(gfx::swapchain_width(), gfx::swapchain_height(), TextureFormat::Depth32, default_depth_clear_state());

			geo_pass_desc.pso_desc.dsv_format = TextureFormat::Depth32;

			m_geometry_pass = rsl::make_unique<RenderPass>(geo_pass_desc);
			m_geometry_pass->set("ViewData", m_cb_view_data.get());
			m_geometry_pass->set("SceneData", m_cb_scene_data.get());
		}

		// This gets called from the client and doesn't directly render anything
		// Instead it cached what needs to be rendered, the actually rendering happens elsewhere
		void SceneRenderer::submit_static_mesh(const TransformComponent& transform, const StaticMesh& mesh, Material* material)
		{
			// For every static mesh, we use one of the pre allocated constant buffers
			// and upload its transform data into it
			s32 constant_buffer_idx = m_draw_lists.size();
			REX_ASSERT_X(constant_buffer_idx < m_per_instance_cbs.size(), "Reached max number of static meshes in a scene. Max: {}", m_per_instance_cbs.size());

			PerInstanceData per_instance_data{};
			per_instance_data.world = transform.world_mat();
			per_instance_data.worldviewproj = per_instance_data.world * m_per_view_data.view_proj;

			auto copy_ctx = new_copy_ctx();
			copy_ctx->update_buffer(m_per_instance_cbs[constant_buffer_idx].get(), &per_instance_data, sizeof(per_instance_data));

			DrawList draw_list{};
			draw_list.vb = mesh.vb();
			draw_list.ib = mesh.ib();
			draw_list.material = material;
			draw_list.per_instance_cb = m_per_instance_cbs[constant_buffer_idx].get();
			m_draw_lists.push_back(draw_list);
		}

		void SceneRenderer::upload_scene_data()
		{
			m_per_view_data.view = glm::transpose(m_scene_data.camera->view_mat());
			m_per_view_data.proj = glm::transpose(m_scene_data.camera->projection_mat());
			m_per_view_data.view_proj = m_per_view_data.view * m_per_view_data.proj;

			m_per_scene_data.light_direction = m_scene_data.light_direction;

			auto copy_ctx = new_copy_ctx();
			copy_ctx->update_buffer(m_cb_view_data.get(), &m_per_view_data, sizeof(m_per_view_data));
			copy_ctx->update_buffer(m_cb_scene_data.get(), &m_per_scene_data, sizeof(m_per_scene_data));
		}

		void SceneRenderer::geometry_pass(RenderContext* ctx)
		{
			m_geometry_pass->bind_to(ctx);
			s32 per_instance_slot = m_geometry_pass->slot("PerInstance");

			m_geometry_pass->sort_draw_lists(m_draw_lists);

			// Loop over all draw lists
			for (s32 i = 0; i < m_draw_lists.size(); ++i)
			{
				const DrawList& drawlist = m_draw_lists[i];

				// Create a new PSO for a draw list if it's required
				// Assign new PSO to pipeline and set its render pass parameters
				m_geometry_pass->bind_material(ctx, drawlist.material);

				ctx->bind_constant_buffer(per_instance_slot, drawlist.per_instance_cb);
				ctx->set_vertex_buffer(drawlist.vb);
				ctx->set_index_buffer(drawlist.ib);

				// Draw
				ctx->draw_indexed(drawlist.ib->count(), 0, 0, 0);
			}

			ctx->execute_on_gpu();
			m_draw_lists.clear();
			
		}
	}
}