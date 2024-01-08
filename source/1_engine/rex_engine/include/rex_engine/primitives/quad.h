#pragma once

#include "rex_engine/primitives/mesh_factory.h"

namespace rex
{
    namespace mesh_factory
    {
		template<typename T>
		MeshData<T> create_quad(f32 x, f32 y, f32 w, f32 h, f32 depth)
		{
			MeshData<T> mesh_data;

			mesh_data.resize_vertices(4);
			mesh_data.resize_indices(6);

			// Position coordinates specified in NDC space.
			mesh_data.vertices()[0] = Vertex(
				x, y - h, depth,
				0.0f, 0.0f, -1.0f,
				1.0f, 0.0f, 0.0f,
				0.0f, 1.0f);

			mesh_data.vertices()[1] = Vertex(
				x, y, depth,
				0.0f, 0.0f, -1.0f,
				1.0f, 0.0f, 0.0f,
				0.0f, 0.0f);

			mesh_data.vertices()[2] = Vertex(
				x + w, y, depth,
				0.0f, 0.0f, -1.0f,
				1.0f, 0.0f, 0.0f,
				1.0f, 0.0f);

			mesh_data.vertices()[3] = Vertex(
				x + w, y - h, depth,
				0.0f, 0.0f, -1.0f,
				1.0f, 0.0f, 0.0f,
				1.0f, 1.0f);

			mesh_data.indices()[0] = 0;
			mesh_data.indices()[1] = 1;
			mesh_data.indices()[2] = 2;

			mesh_data.indices()[3] = 0;
			mesh_data.indices()[4] = 2;
			mesh_data.indices()[5] = 3;

			return mesh_data;
		}
    }
}