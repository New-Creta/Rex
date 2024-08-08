#pragma once

#include "rex_renderer_core/resources/static_mesh.h"

namespace rex
{
	namespace gfx
	{
		class Material;

		// A component holding the static mesh object as well as the material that should be applied on it
		class StaticMeshComponent
		{
		public:
			StaticMeshComponent(StaticMesh&& mesh);

			// Change the material of the component to another one
			void set_material(Material* material);

			// Return the static mesh that's held by the component
			const StaticMesh& static_mesh() const;

		private:
			StaticMesh m_mesh;
			Material* m_material;
		};
	}
}