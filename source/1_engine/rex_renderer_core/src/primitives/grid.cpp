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
    } // namespace mesh_factory
  }
} // namespace rex