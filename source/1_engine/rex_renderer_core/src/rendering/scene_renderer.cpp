#include "rex_renderer_core/rendering/scene_renderer.h"

#include "rex_renderer_core/resources/constant_buffer.h"
#include "rex_renderer_core/gfx/rhi.h"

#include "rex_renderer_core/rendering/camera.h"

#include "rex_renderer_core/gfx/copy_context.h"
#include "rex_renderer_core/gfx/graphics.h"

#include "rex_engine/filesystem/vfs.h"

#include "rex_renderer_core/system/shader_library.h"
#include "rex_renderer_core/scenegraph/scene.h"

namespace rex
{
	namespace gfx
	{
		SceneRenderer::SceneRenderer()
		{
			init_gpu_resources();
			init_render_passes();
		}

		void SceneRenderer::new_frame()
		{
			// Nothing to implement
		}

		void SceneRenderer::set_scene(Scene* scene)
		{
			m_active_scene = scene;
		}
		void SceneRenderer::set_camera(Camera* camera)
		{
			m_active_camera = camera;
		}

		void SceneRenderer::render()
		{
			m_active_scene->render(*this, *m_active_camera);
		}

		void SceneRenderer::begin_scene(const Camera& camera)
		{
			m_current_ctx = m_geometry_pass->begin();

			// 1. Update the per view data.
			// This data consistss of data that's specified to the view (eg. camera)
			update_view_data(m_current_ctx.get(), camera);

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
		}

		void SceneRenderer::end_scene()
		{
			s32 instance_constant_buffer_param_idx = 0;
			m_current_ctx->set_constant_buffer(instance_constant_buffer_param_idx, m_per_instance_cb.get());
			for (DrawList& drawlist : m_draw_lists)
			{
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
			m_per_instance_cb = rhi::create_constant_buffer(sizeof(PerInstanceData) * 100);
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
			geo_pass_desc.pso_desc.depth_stencil_state; // Nothing to set 

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
			PerInstanceData per_instance_data{};
			per_instance_data.world = transform.world_mat();
			per_instance_data.worldviewproj = per_instance_data.world * m_per_view_data.view_perspective;
			s32 offset = 0; // this should grow over time

			auto copy_ctx = new_copy_ctx();
			copy_ctx->update_buffer(m_per_instance_cb.get(), &per_instance_data, sizeof(per_instance_data), offset);
			auto sync_info = copy_ctx->execute_on_gpu();

			DrawList draw_list{};
			draw_list.vb = mesh.vb();
			draw_list.ib = mesh.ib();
			m_draw_lists.push_back(draw_list);
		}

		void SceneRenderer::update_view_data(RenderContext* ctx, const Camera& camera)
		{
			m_per_view_data.view = camera.view_mat();
			m_per_view_data.view = glm::transpose(m_per_view_data.view);
			m_per_view_data.perspective = camera.projection_mat();
			m_per_view_data.view = glm::transpose(m_per_view_data.view);

			m_per_view_data.view_perspective = m_per_view_data.view * m_per_view_data.perspective;

			glm::vec3 eye_pos_w;
			eye_pos_w.x = 15.0f * sinf(0.2f * glm::pi<f32>()) * cosf(1.5f * glm::pi<f32>());
			eye_pos_w.y = 15.0f * cosf(0.2f * glm::pi<f32>());
			eye_pos_w.z = 35.0f * sinf(0.2f * glm::pi<f32>()) * sinf(1.5f * glm::pi<f32>());

			const glm::vec3 pos = glm::vec3(eye_pos_w.x, eye_pos_w.y, eye_pos_w.z);
			const glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
			const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

			glm::mat4 view = glm::lookAt(pos, target, up);
			view = glm::transpose(view); // DirectX backend ( so we have to transpose, expects row major matrices )

			glm::mat4 proj = glm::perspectiveFov(0.25f * glm::pi<f32>(), 1280.0f, 720.0f, 0.1f, 1000.0f);
			proj = glm::transpose(proj); // DirectX backend ( so we have to transpose, expects row major matrices )

			const glm::mat4 view_proj = view * proj;

			m_per_view_data.view = view;
			m_per_view_data.perspective = proj;
			m_per_view_data.view_perspective = view_proj;

			s32 offset = 0;
			auto copy_ctx = new_copy_ctx();
			copy_ctx->update_buffer(m_per_view_cb.get(), &m_per_view_data, sizeof(m_per_view_data), offset);

			s32 per_view_cb_param_idx = 1;
			ctx->set_constant_buffer(per_view_cb_param_idx, m_per_view_cb.get());

			Viewport viewport = { 0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f };
			ctx->set_viewport(viewport);

			ScissorRect rect = { 0, 0, 1280, 720 };
			ctx->set_scissor_rect(rect);
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
	}
}