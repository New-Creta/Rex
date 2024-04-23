#include "rex_directx/mesh.h"

#include "rex_renderer_core/primitive_topology.h"
#include "rex_renderer_core/renderer.h"

namespace rex
{
  namespace renderer
  {
    Mesh::Mesh()
        : m_primitive_topology(PrimitiveTopology::TRIANGLELIST)
    {
    }

    Mesh::~Mesh() = default;

    void Mesh::set_primitive_topology(PrimitiveTopology primitiveToplogy)
    {
      m_primitive_topology = primitiveToplogy;
    }

    void Mesh::set_vertex_buffer(u32 slotID, const ResourceSlot& vertexBuffer, size_t numVerticies)
    {
      m_vertex_buffer_slot_map[slotID] = vertexBuffer;
      m_vertex_num_map[slotID]         = numVerticies;
    }

    void Mesh::set_index_buffer(const ResourceSlot& indexBuffer, size_t numIndicies)
    {
      m_index_buffer_slot = indexBuffer;
      m_num_indicies      = numIndicies;
    }

    PrimitiveTopology Mesh::get_primitive_topology() const
    {
      return m_primitive_topology;
    }

    ResourceSlot Mesh::get_vertex_buffer(u32 slotID) const
    {
      auto itr          = m_vertex_buffer_slot_map.find(slotID);
      auto VertexBuffer = itr != m_vertex_buffer_slot_map.end() ? itr->value : ResourceSlot::make_invalid();

      return VertexBuffer;
    }

    ResourceSlot Mesh::get_index_buffer() const
    {
      return m_index_buffer_slot;
    }

    const Mesh::VertexBufferSlotMap& Mesh::get_vertex_buffers() const
    {
      return m_vertex_buffer_slot_map;
    }

    size_t Mesh::get_index_count() const
    {
      return m_num_indicies;
    }

    size_t Mesh::get_vertex_count() const
    {
      size_t vertex_count = 0;

      VertexNumMap::const_iterator itr = m_vertex_num_map.cbegin();
      while(itr != m_vertex_num_map.cend())
      {
        vertex_count += itr->value;
        itr++;
      }

      return vertex_count;
    }

    void Mesh::draw(u32 instanceCount, u32 startInstance)
    {
      renderer::set_primitive_topology(get_primitive_topology());

      for(auto vertexBuffer: m_vertex_buffer_slot_map)
      {
        renderer::set_vertex_buffer(vertexBuffer.key, vertexBuffer.value);
      }

      auto index_count  = get_index_count();
      auto vertex_count = get_vertex_count();

      if(index_count > 0)
      {
        renderer::set_index_buffer(m_index_buffer_slot);
        renderer::draw_geometry_indexed((u32)index_count, instanceCount, 0u, 0u, startInstance);
      }
      else if(vertex_count > 0)
      {
        renderer::draw_geometry((u32)vertex_count, instanceCount, 0u, startInstance);
      }
    }
  } // namespace renderer
} // namespace rex