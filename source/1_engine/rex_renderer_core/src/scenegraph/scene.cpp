#include "rex_renderer_core/scenegraph/scene.h"

#include "rex_renderer_core/rendering/renderer.h"
#include "rex_renderer_core/rendering/scene_renderer.h"
#include "rex_renderer_core/rendering/camera.h"

#include "rex_renderer_core/components/mesh_component.h"

namespace rex
{
	namespace gfx
	{
		void Scene::render(SceneRenderer& renderer, const Camera& camera)
		{
			renderer.begin_scene(camera);

			render_static_meshes(renderer);

			renderer.end_scene();
		}

		Entity Scene::create_empty_entity(rsl::string_view name)
		{
			entt::entity entt_handle = m_registry.create();
			Entity entity(m_registry, entt_handle, name);
			entity.add_component<TransformComponent>();

			return entity;
		}

		void Scene::render_static_meshes(SceneRenderer& renderer)
		{
			auto entities = m_registry.view<TransformComponent, StaticMeshComponent>();

			for (auto entity : entities)
			{
				auto& smc = entities.get<StaticMeshComponent>(entity);
				auto& transform = entities.get<TransformComponent>(entity);

				renderer.submit_static_mesh(transform, smc.static_mesh());
			}
		}

	}
}