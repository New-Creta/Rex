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
			begin_scene();

			m_scene_data.scene->submit_geo_to_renderer(this);

			end_scene();
		}

		void SceneRenderer::begin_scene()
		{
			m_current_ctx = m_geometry_pass->begin();

			// 1. Update the per view data.
			// This data consistss of data that's specified to the view (eg. camera)
			update_view_data(m_current_ctx.get());

			// 2. Update the data per render pass
			// We currently only have a single render pass so this is not a concern for us right now
			update_pass_data(m_current_ctx.get());

			// 3. Update the data per material
			// Each object in the scene will have their own material
			// We need to make sure all our object's texture views are uploaded to the gpu (eg. textures)
			update_material_data(m_current_ctx.get());

			// 4. Update the per instance data
			// Each object may have data that's specific to each instance of that object (eg. world matrix)
			//update_instance_data(render_ctx.get());			

			// Bind the previously set resources to the pipeline
			bind_resources();
		}

		void SceneRenderer::end_scene()
		{
			s32 instance_constant_buffer_param_idx = 0;
			for (s32 i = 0; i < m_draw_lists.size(); ++i)
			{
				const DrawList& drawlist = m_draw_lists[i];

				m_current_ctx->set_constant_buffer(instance_constant_buffer_param_idx, drawlist.cb);
				m_current_ctx->set_vertex_buffer(drawlist.vb);

				// submit index buffer
				m_current_ctx->set_index_buffer(drawlist.ib);

				m_current_ctx->draw_indexed(drawlist.ib->count(), 0, 0, 0);
			}

			m_current_ctx.return_to_pool();
			m_draw_lists.clear();
		}

		void SceneRenderer::init_gpu_resources()
		{
			m_per_view_cb = rhi::create_constant_buffer(sizeof(PerViewData));
			m_per_pass_cb = rhi::create_constant_buffer(sizeof(PerPassData));
			m_per_material_cb = rhi::create_constant_buffer(1_kib);
			s32 num_instances_supported = 100;
			m_per_instance_cbs.reserve(num_instances_supported);
			for (s32 i = 0; i < num_instances_supported; ++i)
			{
				m_per_instance_cbs.push_back(rhi::create_constant_buffer(sizeof(PerInstanceData)));
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
			// Define the the blend settings
			geo_pass_desc.pso_desc.blend_state; // Nothing to set

			// Define the depth stencil settings
			geo_pass_desc.pso_desc.depth_stencil_state.depth_enable = true;

			// Define the raster state
			geo_pass_desc.pso_desc.raster_state; // Nothing to set

			// Define the input layout
			geo_pass_desc.pso_desc.input_layout = 
			{
				{ ShaderSemantic::Position, ShaderParameterType::Float3    },
				{ ShaderSemantic::Normal,   ShaderParameterType::Float3    },
				{ ShaderSemantic::Color,    ShaderParameterType::Float4    } 
			};

			// Define the shaders
			geo_pass_desc.pso_desc.shader_pipeline.vs = shader_lib::load(vfs::abs_path(MountingPoint::EngineShaders, "basic_vertex.hlsl"), ShaderType::Vertex);
			geo_pass_desc.pso_desc.shader_pipeline.ps = shader_lib::load(vfs::abs_path(MountingPoint::EngineShaders, "basic_pixel.hlsl"), ShaderType::Pixel);

			// Define the primitive topology
			geo_pass_desc.pso_desc.primitive_topology = PrimitiveTopologyType::Triangle;

			m_geometry_pass = rsl::make_unique<RenderPass>(geo_pass_desc);
		}

		void SceneRenderer::submit_static_mesh(const TransformComponent& transform, const StaticMesh& mesh)
		{
			// update constant buffer
			s32 constant_buffer_idx = m_draw_lists.size();

			PerInstanceData per_instance_data{};
			per_instance_data.world = transform.world_mat();
			per_instance_data.worldviewproj = per_instance_data.world * m_per_view_data.view_proj;

			auto copy_ctx = new_copy_ctx();
			copy_ctx->update_buffer(m_per_instance_cbs[constant_buffer_idx].get(), &per_instance_data, sizeof(per_instance_data));
			auto sync_info = copy_ctx->execute_on_gpu();

			DrawList draw_list{};
			draw_list.vb = mesh.vb();
			draw_list.ib = mesh.ib();
			draw_list.cb = m_per_instance_cbs[constant_buffer_idx].get();
			m_draw_lists.push_back(draw_list);
		}

		void SceneRenderer::update_view_data(RenderContext* ctx)
		{
			m_per_view_data.view = glm::transpose(m_scene_data.camera->view_mat());
			m_per_view_data.proj = glm::transpose(m_scene_data.camera->projection_mat());
			m_per_view_data.view_proj = m_per_view_data.view * m_per_view_data.proj;

			auto copy_ctx = new_copy_ctx();
			copy_ctx->update_buffer(m_per_view_cb.get(), &m_per_view_data, sizeof(m_per_view_data));
		}

		void SceneRenderer::update_pass_data(RenderContext* ctx)
		{
			// Nothing to implement yet
		}

		void SceneRenderer::update_material_data(RenderContext* ctx)
		{

		}

		void SceneRenderer::update_instance_data(RenderContext* ctx)
		{

		}

		void SceneRenderer::bind_resources()
		{
			s32 per_view_cb_param_idx = 1;
			m_current_ctx->set_constant_buffer(per_view_cb_param_idx, m_per_view_cb.get());

			f32 viewport_width = static_cast<f32>(m_scene_data.viewport_width);
			f32 viewport_height = static_cast<f32>(m_scene_data.viewport_height);
			Viewport viewport = { 0.0f, 0.0f, viewport_width, viewport_height, 0.0f, 1.0f };
			m_current_ctx->set_viewport(viewport);

			ScissorRect rect = { 0, 0, 1280, 720 };
			m_current_ctx->set_scissor_rect(rect);
		}
	}
}