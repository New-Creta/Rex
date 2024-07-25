#include "rex_renderer_core/scenegraph/scene.h"

namespace rex
{
	namespace gfx
	{
		Entity Scene::create_empty_entity(rsl::string_view name)
		{
			entt::entity entt_handle = m_registry.create();
			return Entity(m_registry, entt_handle, name);
		}

	}
}