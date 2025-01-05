#pragma once

#include "entt/entt.hpp"

#include "rex_std/string.h"
#include "rex_std/string_view.h"
#include "rex_std/utility.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
	namespace gfx
	{
		// This class is equivalent to Unreal's actor class and Unity's gameobject
		// It's the class used to reference an object in the world
		// However the class itself is empty and components are added to it
		class Entity
		{
		public:
			// Construct an empty entity
			Entity();
			// Construct an entity with a given name
			Entity(entt::registry& registry, entt::entity entity, rsl::string_view name);

			// Add a new component to an entity
			template <typename Component, typename ... Args>
			Component& add_component(Args&&... args)
			{
				REX_ASSERT_X(m_entity_handle != entt::null, "Entity does not have a entt handle and therefore cannot have a component");
				REX_ASSERT_X(m_registry, "No registry assigned to this entity, therefore cannot add a component");
				return m_registry->emplace<Component>(m_entity_handle, rsl::forward<Args>(args)...);
			}

			// Get a component from an entity
			template <typename Component>
			const Component& component() const
			{
				return m_registry->get<Component>(m_entity_handle);
			}
			template <typename Component>
			Component& component()
			{
				return m_registry->get<Component>(m_entity_handle);
			}
			
			// Return the name of the entity
			rsl::string_view name() const;

			entt::entity handle() const;

		private:
			entt::entity m_entity_handle;	// The entity handle which is what entt needs to operate on the entity
			entt::registry* m_registry;		// The registry the entity is stored in
			rsl::string m_name;				// The name of the entity
		};
	}
}