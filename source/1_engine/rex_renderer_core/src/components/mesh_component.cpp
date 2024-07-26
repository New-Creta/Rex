#include "rex_renderer_core/components/mesh_component.h"

namespace rex
{
	namespace gfx
	{
		StaticMeshComponent::StaticMeshComponent(StaticMesh&& mesh)
			: m_mesh(rsl::move(mesh))
		{}

		void StaticMeshComponent::set_material(Material* material)
		{
			m_material = material;
		}


	}
}