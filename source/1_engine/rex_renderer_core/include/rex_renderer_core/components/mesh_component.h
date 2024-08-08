#pragma once

#include "rex_renderer_core/resources/static_mesh.h"

namespace rex
{
	namespace gfx
	{
		class Material;

		class StaticMeshComponent
		{
		public:
			StaticMeshComponent(StaticMesh&& mesh);

			void set_material(Material* material);

			const StaticMesh& static_mesh() const;

		private:
			StaticMesh m_mesh;
			Material* m_material;
		};
	}
}