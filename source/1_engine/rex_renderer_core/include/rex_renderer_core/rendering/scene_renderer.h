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

		// This is just stub data, as we'll have multiple passes, we'll have data for those
		struct PerPassData
		{

		};

		struct DrawList
		{
			VertexBuffer* vb;
			IndexBuffer* ib;
			ConstantBuffer* cb;
		};

		struct PerInstanceData
		{
			glm::mat4 world;
			glm::mat4 worldviewproj;
		};

		struct SceneData
		{
			Scene* scene;
			const Camera* camera;
			s32 viewport_width;
			s32 viewport_height;
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

			void update_view_data(RenderContext* ctx);
			void update_pass_data(RenderContext* ctx);
			void update_material_data(RenderContext* ctx);
			void update_instance_data(RenderContext* ctx);

			void bind_resources();

		private:
			SceneData m_scene_data;

			rsl::unique_ptr<ConstantBuffer> m_per_view_cb;
			rsl::unique_ptr<ConstantBuffer> m_per_pass_cb;
			rsl::unique_ptr<ConstantBuffer> m_per_material_cb;
			rsl::vector<rsl::unique_ptr<ConstantBuffer>> m_per_instance_cbs;

			PerViewData m_per_view_data;

			rsl::unique_ptr<RenderPass> m_geometry_pass;

			rsl::vector<DrawList> m_draw_lists;
			rex::ScopedPoolObject<RenderContext> m_current_ctx;
			rex::ScopedPoolObject<SyncInfo> m_last_copy_sync_info;
		};
	}
}