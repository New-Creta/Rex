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
		class Entity
		{
		public:
			Entity();
			Entity(entt::registry& registry, entt::entity entity, rsl::string_view name);

			template <typename Component, typename ... Args>
			Component& add_component(Args&&... args)
			{
				REX_ASSERT_X(m_entity_handle != entt::null, "Entity does not have a entt handle and therefore cannot have a component");
				REX_ASSERT_X(m_registry, "No registry assigned to this entity, therefore cannot add a component");
				return m_registry->emplace<Component>(m_entity_handle, rsl::forward<Args>(args)...);
			}

		private:
			entt::entity m_entity_handle;
			entt::registry* m_registry;
			rsl::string m_name;
		};
	}
}