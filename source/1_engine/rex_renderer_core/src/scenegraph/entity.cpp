#include "rex_renderer_core/scenegraph/entity.h"

namespace rex
{
	namespace gfx
	{
		Entity::Entity()
			: m_entity_handle(entt::null)
		{}

		Entity::Entity(entt::registry& registry, entt::entity entityHandle, rsl::string_view name)
			: m_entity_handle(entityHandle)
			, m_registry(&registry)
			, m_name(name)
		{}
	}
}