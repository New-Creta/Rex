#pragma once

#include "rex_engine/engine/types.h"

#include "rex_renderer_core/resource_slot.h"
#include "rex_renderer_core/primitive_topology.h"

#include "rex_std/map.h"
#include "rex_std/memory.h"

namespace rex
{
  namespace renderer
  {
    class Mesh
    {
    public:
      using VertexBufferSlotMap = rsl::map<u32, ResourceSlot>;
      using VertexNumMap = rsl::map<u32, size_t>;

      Mesh();
      ~Mesh();

      void                          set_primitive_topology(PrimitiveTopology primitiveToplogy);
      void                          set_vertex_buffer(u32 slotID, const ResourceSlot& vertexBuffer, size_t numVerticies);
      void                          set_index_buffer(const ResourceSlot& indexBuffer, size_t numIndicies);

      PrimitiveTopology             get_primitive_topology() const;
      ResourceSlot                  get_vertex_buffer(u32 slotID) const;
      ResourceSlot                  get_index_buffer() const;
      const VertexBufferSlotMap&    get_vertex_buffers() const;

      /**
       * Get the number if indices in the index buffer.
       * If no index buffer is bound to the mesh, this function returns 0.
       */
      size_t                        get_index_count() const;

      /**
       * Get the number of vertices in the mesh.
       * If this mesh does not have a vertex buffer, the function returns 0.
       */
      size_t                        get_vertex_count() const;

      /**
       * Draw the mesh.
       *
       * @param instanceCount The number of instances to draw.
       * @param startInstance The offset added to the instance ID when reading from the instance buffers.
       */
      void                          draw(u32 instanceCount = 1, u32 startInstance = 0);

    private:
      VertexBufferSlotMap           m_vertex_buffer_slot_map;
      VertexNumMap                  m_vertex_num_map;
      ResourceSlot                  m_index_buffer_slot;
      size_t                        m_num_indicies;

      PrimitiveTopology             m_primitive_topology;
    };
  } // namespace renderer
} // namespace rex