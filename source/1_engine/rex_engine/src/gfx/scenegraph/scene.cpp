#include "rex_engine/gfx/scenegraph/scene.h"

#include "rex_engine/gfx/rendering/renderer.h"
#include "rex_engine/gfx/rendering/scene_renderer.h"
#include "rex_engine/gfx/rendering/camera.h"

#include "rex_engine/gfx/components/mesh_component.h"

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

				renderer->submit_static_mesh(transform, smc.static_mesh(), smc.material());
			}
		}

		Entity Scene::add_entity(rsl::string_view name)
		{
			// Every entity needs a transform component anyway, so we add it by default
			entt::entity entt_handle = m_registry.create();
			Entity entity(m_registry, entt_handle, name);
			entity.add_component<TransformComponent>();
			m_entity_name_map[name] = entity;
			return entity;
		}

		Entity Scene::find_entity(rsl::string_view name)
		{
			if (!m_entity_name_map.contains(name))
			{
				return Entity();
			}

			return m_entity_name_map.at(name);
		}

		void Scene::remove_entity(Entity& entity)
		{
			m_registry.destroy(entity.handle());
		}
	}
}