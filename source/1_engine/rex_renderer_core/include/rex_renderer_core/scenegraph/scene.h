#pragma once

#include "rex_renderer_core/scenegraph/entity.h"

#include "rex_std/string_view.h"
#include "entt/entt.hpp"

namespace rex
{
	namespace gfx
	{
		class Scene
		{
		public:
			Entity create_empty_entity(rsl::string_view name = "Empty");

		private:
			entt::registry m_registry;
		};
	}
}