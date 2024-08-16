#include "rex_renderer_core/components/mesh_component.h"

namespace rex
{
	namespace gfx
	{
		StaticMeshComponent::StaticMeshComponent(StaticMesh&& mesh)
			: m_mesh(rsl::move(mesh))
			, m_material(nullptr)
		{}

		void StaticMeshComponent::set_material(Material* material)
		{
			m_material = material;
		}

		Material* StaticMeshComponent::material()
		{
			return m_material;
		}

		const StaticMesh& StaticMeshComponent::static_mesh() const
		{
			return m_mesh;
		}
	}
}