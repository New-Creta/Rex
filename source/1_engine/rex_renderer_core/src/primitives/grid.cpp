#include "rex_renderer_core/primitives/grid.h"

#include "rex_renderer_core/gfx/rhi.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  namespace gfx
  {
    namespace mesh_factory
    {
      StaticMesh create_grid(s16 cellSize, s32 cellsPerLine, f32 lineWidth)
      {
        REX_ASSERT_X(cellsPerLine > 0, "cannot create a grid with a non positive nr of cells per row");
        REX_ASSERT_X(cellSize > 0, "cannot create a grid with a non positive cell size");
        REX_ASSERT_X(lineWidth < (cellSize / 2), "the linewidth of a grid cannot exceed half the cell size");

        f32 width = cellSize * cellsPerLine;
        f32 depth = cellSize * cellsPerLine;

        f32 min_x = -width / 2;
        f32 max_x = width / 2;
        f32 min_z = -depth / 2;
        f32 max_z = depth / 2;

        f32 half_line_width = lineWidth / 2;

        s32 num_vertices = cellsPerLine * 4; // you need 4 vertices to represent a single cell
        s32 num_lines = (cellsPerLine * 2) + 2; // the number of lines drawn is equal to double the cells per line + 2
        s32 num_indices = num_lines * 6; // You need 6 indices to draw a single rectangle, multiply this by the number of lines

        rsl::vector<VertexPosNormTex> vertices;
        vertices.reserve(num_vertices);
        rsl::vector<u16> indices;
        indices.reserve(num_lines);
        for (s32 row = 0; row < width; row += cellSize)
        {
          u16 start_vtx = vertices.size();
          f32 uv_coordinate = row / width;
          vertices.emplace_back(glm::vec3(0.0f,  0.0f, row - half_line_width),  glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, uv_coordinate));
          vertices.emplace_back(glm::vec3(0.0f,  0.0f, row + half_line_width),  glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, uv_coordinate));
          vertices.emplace_back(glm::vec3(width, 0.0f, row - half_line_width),  glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, uv_coordinate));
          vertices.emplace_back(glm::vec3(width, 0.0f, row + half_line_width),  glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, uv_coordinate));

          indices.push_back(start_vtx + 0);
          indices.push_back(start_vtx + 1);
          indices.push_back(start_vtx + 2);
          indices.push_back(start_vtx + 1);
          indices.push_back(start_vtx + 3);
          indices.push_back(start_vtx + 2);
        }

        for (s32 column = 0; column < depth; column += cellSize)
        {
          u16 start_vtx = vertices.size();
          f32 uv_coordinate = column / width;
          vertices.emplace_back(glm::vec3(column - half_line_width, 0.0f, 0.0f),  glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec2(uv_coordinate, 0.0f));
          vertices.emplace_back(glm::vec3(column + half_line_width, 0.0f, 0.0f),  glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec2(uv_coordinate, 0.0f));
          vertices.emplace_back(glm::vec3(column - half_line_width, 0.0f, depth), glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec2(uv_coordinate, 0.0f));
          vertices.emplace_back(glm::vec3(column + half_line_width, 0.0f, depth), glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec2(uv_coordinate, 0.0f));

          indices.push_back(start_vtx + 0);
          indices.push_back(start_vtx + 1);
          indices.push_back(start_vtx + 2);
          indices.push_back(start_vtx + 1);
          indices.push_back(start_vtx + 3);
          indices.push_back(start_vtx + 2);
        }

        rsl::unique_ptr<VertexBuffer> vb = rhi::create_vertex_buffer(vertices.size(), sizeof(vertices.front()), vertices.data());
        rsl::unique_ptr<IndexBuffer> ib = rhi::create_index_buffer(indices.size(), IndexBufferFormat::Uint16, indices.data());

        return StaticMesh(rsl::move(vb), rsl::move(ib));
      }

      //StaticMesh create_grid(f32 width, f32 depth, s16 cellSize)
      //{
      //  REX_ASSERT_X(width > 0, "cannot create a grid with a non positive width");
      //  REX_ASSERT_X(depth > 0, "cannot create a grid with a non positive depth");
      //  REX_ASSERT_X(cellSize > 0, "cannot create a grid with a non positive cell size");

      //  f32 min_x = -width / 2;
      //  f32 max_x = width / 2;
      //  f32 min_z = -depth / 2;
      //  f32 max_z = depth / 2;

      //  
      //}

      //StaticMesh create_grid(f32 width, f32 depth, s16 m, s16 n)
      //{
      //  REX_ASSERT_X(width > 0, "cannot create a grid of 0 width");
      //  REX_ASSERT_X(depth > 0, "cannot create a grid of 0 depth");
      //  REX_ASSERT_X(m > 0, "cannot create a grid with 0 width elements");
      //  REX_ASSERT_X(n > 0, "cannot create a grid of 0 depth elements");

      //  MeshData16 mesh_data;

      //  const s32 vertex_count = m * n;
      //  const s32 face_count = (m - 1) * (n - 1) * 2;

      //  //
      //  // Create the vertices.
      //  //

      //  const f32 half_width = 0.5f * width;
      //  const f32 half_depth = 0.5f * depth;

      //  const f32 dx = width / static_cast<f32>(n - 1);
      //  const f32 dz = depth / static_cast<f32>(m - 1);

      //  const f32 du = 1.0f / static_cast<f32>(n - 1);
      //  const f32 dv = 1.0f / static_cast<f32>(m - 1);

      //  mesh_data.reserve_vertices(vertex_count);
      //  for (u16 i = 0; i < m; ++i)
      //  {
      //    const f32 z = half_depth - static_cast<f32>(i) * dz;
      //    for (u16 j = 0; j < n; ++j)
      //    {
      //      const f32 x = -half_width + static_cast<f32>(j) * dx;

      //      glm::vec3 const position = glm::vec3(x, 0.0f, z);
      //      glm::vec3 const normal = glm::vec3(0.0f, 1.0f, 0.0f);
      //      glm::vec4 const col = glm::vec4(normal, 0.0f);

      //      mesh_data.insert_vertex(i * n + j, position, normal, col);
      //    }
      //  }

      //  //
      //  // Create the indices.
      //  //
      //  const s32 num_indices = face_count * 3;
      //  mesh_data.reserve_indices(face_count * 3); // 3 indices per face

      //  // Iterate over each quad and compute indices.
      //  u16 k = 0;
      //  for (u16 i = 0; i < m - 1; ++i)
      //  {
      //    for (u16 j = 0; j < n - 1; ++j)
      //    {
      //      mesh_data.insert_index(k + 0, i * n + j);
      //      mesh_data.insert_index(k + 1, i * n + j + 1);
      //      mesh_data.insert_index(k + 2, (i + 1) * n + j);

      //      mesh_data.insert_index(k + 3, (i + 1) * n + j);
      //      mesh_data.insert_index(k + 4, i * n + j + 1);
      //      mesh_data.insert_index(k + 5, (i + 1) * n + j + 1);

      //      k += 6; // next quad
      //    }
      //  }

      //  rsl::unique_ptr<VertexBuffer> vb = rhi::create_vertex_buffer(mesh_data.vertices().size(), mesh_data.vertex_size(), mesh_data.vertices().data());
      //  rsl::unique_ptr<IndexBuffer> ib = rhi::create_index_buffer(mesh_data.indices().size(), IndexBufferFormat::Uint16, mesh_data.indices().data());

      //  return StaticMesh(rsl::move(vb), rsl::move(ib));
      //}
    } // namespace mesh_factory
  }
} // namespace rex