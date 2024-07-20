#include "pokemon/map.h"

#include "rex_renderer_core/primitives/box.h"

namespace pokemon
{
	Map::Map(rsl::string_view name)
		: m_name(name)
	{}

	void Map::load_hardcoded_scene()
	{
		auto box_mesh_data = rex::gfx::mesh_factory::create_box(1.0f, 1.0f, 1.0f);

		m_scene = rsl::make_unique<rex::gfx::Scene>();
	}
}