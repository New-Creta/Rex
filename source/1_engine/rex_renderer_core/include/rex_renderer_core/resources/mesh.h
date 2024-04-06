#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/memory/blob.h"
#include "rex_engine/memory/blob_view.h"
#include "rex_renderer_core/rendering/index_buffer_format.h"
#include "rex_renderer_core/resource_management/resource_slot.h"
#include "rex_renderer_core/resources/index_buffer.h"
#include "rex_renderer_core/resources/vertex_buffer.h"
#include "rex_renderer_core/system/renderer.h"
#include "rex_std/bonus/string/stack_string.h"
#include "rex_std/bonus/utility.h"
#include "rex_std/memory.h"
#include "rex_std/unordered_map.h"

namespace rex
{
  namespace renderer
  {
    /*
     * Defines a subrange of geometry in a MeshGeometry. This is for when multiple
     * geometries are stored in one vertex and index buffer. It provides the offsets
     * and data needed to draw a subset of geometry stores in the vertex and index
     * buffers so that we can implement the technique described in the following illustration.
     *
     *
     * +-------------------+   +----------------+   +----------------------+
     * | Sphere = {VB, IB} |   | Box = {VB, IB} |   | Cyclinder = {VB, IB} |
     * +-------------------+   +----------------+   +----------------------+
     *
     *
     *                 Global Vertex Buffer
     * +-----------------+--------------+--------------------+
     * | Sphere vertices | Box vertices | Cyclinder vertices |
     * +-----------------+--------------+--------------------+
     * |                 |              |
     * |                 |              |
     * |                 |              |
     * 0                 |         first cylinder vertex pos
     *               first box vertex pos
     *
     *
     *                 Global Index Buffer
     * +----------------+-------------+-------------------+
     * | Sphere indices | Box indices | Cyclinder indices |
     * +----------------+-------------+-------------------+
     * |<-------------->|<----------->|<------------------>
     * | num sphere     | num box     | num cylinder
     * |  indices       |  indices    |  indices
     * |                |             |
     * |                |             |
     * 0                |          first cylinder index
     *               first box index
     *
     *
     * Concatination of several vertex buffers into one large vertex buffer
     * Concatination of several index buffers into one large index buffer
     *
     */

    struct Submesh
    {
      s32 index_count          = 0;
      s32 start_index_location = 0;
      s32 base_vertex_location = 0;
    };

    class Mesh
    {
    public:
      Mesh(rsl::string_view name, rhi::VertexBufferDesc vbd, rhi::IndexBufferDesc ibd);

    public:
      void add_submesh(rsl::string_view name, const Submesh& subMesh);
      void add_submesh(rsl::string_view name, s32 indexCount, s32 startIndexLocation, s32 baseVertexLocation = 0);

      const Submesh* submesh(rsl::string_view name) const;

      const rhi::VertexBufferDesc* vb() const;
      const rhi::IndexBufferDesc* ib() const;

    private:
      // Give it a name so we can look it up by name.
      rsl::medium_stack_string m_name;

      rhi::VertexBufferDesc m_vbd;
      rhi::IndexBufferDesc m_ibd;

      // A Mesh may store multiple geometries in one vertex/index buffer.
      // Use this container to define the Submesh geometries so we can draw
      // the Submeshes individually.
      rsl::unordered_map<rsl::small_stack_string, Submesh> m_submesh_map;
    };
  } // namespace renderer
} // namespace rex