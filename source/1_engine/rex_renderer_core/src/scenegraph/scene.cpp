#include "rex_renderer_core/scenegraph/scene.h"

#include "rex_renderer_core/rendering/renderer.h"
#include "rex_renderer_core/rendering/scene_renderer.h"
#include "rex_renderer_core/rendering/camera.h"

namespace rex
{
	namespace gfx
	{
		void Scene::render(SceneRenderer& renderer, const Camera& camera)
		{
			renderer.begin_scene(camera);



			renderer.end_scene();
		}

		Entity Scene::create_empty_entity(rsl::string_view name)
		{
			entt::entity entt_handle = m_registry.create();
			return Entity(m_registry, entt_handle, name);
		}

	}
}