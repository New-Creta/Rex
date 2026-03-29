#pragma once

#include "rex_engine/gfx/resources/constant_buffer.h"

#include "rex_std/vector.h"

#include "rex_engine/gfx/resources/static_mesh.h"
#include "rex_engine/components/transform_component.h"

#include "rex_engine/gfx/rendering/constant_buffers.h"
#include "rex_engine/gfx/rendering/render_pass.h"
#include "rex_engine/gfx/rendering/renderer.h"
#include "rex_engine/gfx/resources/texture_2d.h"

namespace rex
{
	namespace gfx
	{
		class Scene;
		class Camera;

		// This holds data given by the client, to the scene renderer so it can perform the rendering commmands
		struct SceneData
		{
			Scene* scene;				// The active scene we're rendering
			const Camera* camera;		// The camera we're using to render the scene
			s32 viewport_width;			// The width of the viewport
			s32 viewport_height;		// The height of the viewport
			glm::vec3 light_direction;	// The light direction of the scene
		};

		class SceneRenderer : public Renderer
		{
		public:
			SceneRenderer();

			// Prepare a new frame for the renderer
			void new_frame() override;
			// Render a new frame
			void render() override;

			// Update the scene data of the renderer
			// This is called from the client that holds the information
			// required to update the scene renderer with what to render
			void update_scene_data(const SceneData& sceneData);

			// Submit a static mesh to the renderer
			void submit_static_mesh(const TransformComponent& transform, const StaticMesh& mesh, Material* material);

		private:
			// Initialize the gpu resources required by the renderer
			void init_gpu_resources();
			// Initialize the render passes of the renderer
			void init_render_passes();

			// Initialize the geometry render pass
			void init_geometry_render_pass();

			// Upload the scene data to the gpu
			void upload_scene_data();
			// Go over the pending draw list, through ever pass, and render it to the screen
			void flush_draw_lists();

			// Perform the geometry pass
			void geometry_pass(RenderContext* ctx);

		private:
			// Data coming in from the runtime about the scene, this is not processed yet
			SceneData m_scene_data;
			// Structure that directly maps onto the shader structure holding the scene data
			PerSceneData m_per_scene_data;
			PerViewData m_per_view_data;

			// The GPU resources we use to hold data for the different constant buffers
			rsl::unique_ptr<ConstantBuffer> m_cb_scene_data;
			rsl::unique_ptr<ConstantBuffer> m_cb_view_data;
			rsl::vector<rsl::unique_ptr<ConstantBuffer>> m_per_instance_cbs;

			// Render passes of the scene renderer
			rsl::unique_ptr<RenderPass> m_geometry_pass;

			// Data holding the buffers required for drawing a single piece of geometry
			rsl::vector<DrawList> m_draw_lists;

			// Max number of objects we support in a single scene
			constexpr static s32 s_max_number_of_objects_per_scene = 100;
		};
	}
}