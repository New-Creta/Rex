#pragma once

#include "rex_engine/entities/entity.h"

#include "rex_std/string_view.h"
#include "entt/entt.hpp"
#include "rex_std/unordered_map.h"


namespace rex
{
	namespace gfx
	{
		class SceneRenderer;
		class Camera;

		class Scene
		{
		public:
			virtual ~Scene() = default;

			// Submit the scene's geometry to the scene renderer
			void submit_geo_to_renderer(SceneRenderer* renderer);

			// Add a new entity to the scene.
			// This entity has a transform component added automatically
			Entity add_entity(rsl::string_view name = "Empty");

			Entity find_entity(rsl::string_view name);

			void remove_entity(Entity& entity);

		private:
			entt::registry m_registry;  // the entity registery is owned by the scene
			rsl::unordered_map<rsl::string_view, Entity> m_entity_name_map;
		};
	}
}