#pragma once

#include "rex_renderer_core/resources/constant_buffer.h"

namespace rex
{
	namespace gfx
	{
		class Scene;
		class Camera;

		class SceneRenderer
		{
		public:
			SceneRenderer();

			void begin_scene(const Camera& camera);
			void end_scene();

		private:
			void update_view_data(const Camera& camera);
			void update_pass_data();
			void update_material_data();
			void update_instance_data();

		private:
			Scene* m_active_scene;

			rsl::unique_ptr<ConstantBuffer> m_per_view_cb;
			rsl::unique_ptr<ConstantBuffer> m_per_pass_cb;
			rsl::unique_ptr<ConstantBuffer> m_per_material_cb;
			rsl::unique_ptr<ConstantBuffer> m_per_instance_cb;
		};
	}
}