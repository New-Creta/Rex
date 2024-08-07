#pragma once

#include "rex_renderer_core/resources/constant_buffer.h"

#include "rex_std/vector.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "rex_renderer_core/resources/static_mesh.h"
#include "rex_renderer_core/components/transform_component.h"

#include "rex_renderer_core/rendering/render_pass.h"
#include "rex_renderer_core/rendering/renderer.h"

namespace rex
{
	namespace gfx
	{
		class Scene;
		class Camera;

		struct PerViewData
		{
			glm::mat4 view;
			glm::mat4 proj;
			glm::mat4 view_proj;
		};

		struct PerSceneData
		{
			glm::vec3 light_direction;
		};

		struct PerInstanceData
		{
			glm::mat4 world;
			glm::mat4 worldviewproj;
		};

		struct SceneData
		{
			Scene* scene;								// The active scene we're rendering
			const Camera* camera;				// The camera we're using to render the scene
			s32 viewport_width;					// The width of the viewport
			s32 viewport_height;				// The height of the viewport
			glm::vec3 light_direction;	// The light direction of the scene
		};

		class SceneRenderer : public Renderer
		{
		public:
			SceneRenderer();

			void new_frame() override;
			void render() override;

			void update_scene_data(const SceneData& sceneData);

			void begin_scene();
			void end_scene();

			void submit_static_mesh(const TransformComponent& transform, const StaticMesh& mesh);

		private:
			void init_gpu_resources();
			void init_render_passes();

			void init_geometry_render_pass();

			void upload_scene_data();

			void bind_resources();

			void geometry_pass();

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

			// Current context used for recording gpu commands
			rex::ScopedPoolObject<RenderContext> m_current_ctx;
		};
	}
}