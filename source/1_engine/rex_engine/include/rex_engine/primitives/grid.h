#pragma once

#include "rex_engine/primitives/mesh_factory.h"

namespace rex
{
    namespace mesh_factory
    {
		template<typename T>
		MeshData<T> create_grid(f32 width, f32 depth, u32 m, u32 n)
		{
			MeshData<T> mesh_data;

			u32 vertex_count = m * n;
			u32 face_count = (m - 1) * (n - 1) * 2;

			//
			// Create the vertices.
			//

			f32 half_width = 0.5f * width;
			f32 half_depth = 0.5f * depth;

			f32 dx = width / (n - 1);
			f32 dz = depth / (m - 1);

			f32 du = 1.0f / (n - 1);
			f32 dv = 1.0f / (m - 1);

			mesh_data.resize_vertices(vertex_count);
			for (u32 i = 0; i < m; ++i)
			{
				f32 z = half_depth - i * dz;
				for (u32 j = 0; j < n; ++j)
				{
					f32 x = -half_width + j * dx;

					mesh_data.vertices()[i * n + j].position = glm::vec3(x, 0.0f, z);
					mesh_data.vertices()[i * n + j].normal = glm::vec3(0.0f, 1.0f, 0.0f);
					mesh_data.vertices()[i * n + j].tangent = glm::vec3(1.0f, 0.0f, 0.0f);

					// Stretch texture over grid.
					mesh_data.vertices()[i * n + j].uv.x = j * du;
					mesh_data.vertices()[i * n + j].uv.y = i * dv;
				}
			}

			//
			// Create the indices.
			//

			mesh_data.resize_indices(face_count * 3); // 3 indices per face

			// Iterate over each quad and compute indices.
			u32 k = 0;
			for (u32 i = 0; i < m - 1; ++i)
			{
				for (u32 j = 0; j < n - 1; ++j)
				{
					mesh_data.indices()[k] = i * n + j;
					mesh_data.indices()[k + 1] = i * n + j + 1;
					mesh_data.indices()[k + 2] = (i + 1) * n + j;

					mesh_data.indices()[k + 3] = (i + 1) * n + j;
					mesh_data.indices()[k + 4] = i * n + j + 1;
					mesh_data.indices()[k + 5] = (i + 1) * n + j + 1;

					k += 6; // next quad
				}
			}

			return mesh_data;
		}
    }
}