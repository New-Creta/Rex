#include "rex_renderer_core/scenegraph/scene.h"

#include "rex_renderer_core/rendering/renderer.h"
#include "rex_renderer_core/rendering/scene_renderer.h"
#include "rex_renderer_core/rendering/camera.h"

#include "rex_renderer_core/components/mesh_component.h"

namespace rex
{
	namespace gfx
	{
		void Scene::submit_geo_to_renderer(SceneRenderer* renderer)
		{
			REX_ASSERT_X(renderer != nullptr, "Renderer is nullptr. Cannot submit geometry");

			auto entities = m_registry.view<TransformComponent, StaticMeshComponent>();

			for (auto entity : entities)
			{
				auto& smc = entities.get<StaticMeshComponent>(entity);
				auto& transform = entities.get<TransformComponent>(entity);

				renderer->submit_static_mesh(transform, smc.static_mesh());
			}
		}

		Entity Scene::add_entity(rsl::string_view name)
		{
			// Every entity needs a transform component anyway, so we add it by default
			entt::entity entt_handle = m_registry.create();
			Entity entity(m_registry, entt_handle, name);
			entity.add_component<TransformComponent>();

			return entity;
		}
	}
}