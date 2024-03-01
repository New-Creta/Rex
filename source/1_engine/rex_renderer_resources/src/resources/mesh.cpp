#include "rex_renderer_resources/resources/mesh.h"

#include "rex_renderer_resources/log.h"

namespace rex
{
  namespace renderer
  {
    //-------------------------------------------------------------------------
    Mesh::Mesh(rsl::string_view name, const VertexBufferDesc& vbd, const IndexBufferDesc& ibd)
        : m_name(name)
        , m_vbd(vbd)
        , m_ibd(ibd)
    {
    }

    //-------------------------------------------------------------------------
    void Mesh::add_submesh(rsl::string_view name, const Submesh& subMesh)
    {
      if(m_submesh_map.find(name) != rsl::cend(m_submesh_map))
      {
        REX_ERROR(LogRendererResources, "Submesh with name ({}) already added as a draw argument", name.data());
        return;
      }

      m_submesh_map.emplace(name, subMesh);
    }

    //-------------------------------------------------------------------------
    void Mesh::add_submesh(rsl::string_view name, s32 indexCount, s32 startIndexLocation, s32 baseVertexLocation)
    {
      add_submesh(name, {indexCount, startIndexLocation, baseVertexLocation});
    }

    //-------------------------------------------------------------------------
    const Submesh* Mesh::submesh(rsl::string_view name) const
    {
      if(m_submesh_map.find(name) == rsl::cend(m_submesh_map))
      {
        REX_ERROR(LogRendererResources, "Submesh with name ({}) not found as a draw argument", name.data());
        return nullptr;
      }

      return &m_submesh_map.at(name);
    }

    //-------------------------------------------------------------------------
    rsl::string_view Mesh::name() const
    {
      return m_name;
    }

    //-------------------------------------------------------------------------
    const ResourceSlot& Mesh::vertex_buffer_slot() const
    {
      return m_vbd.slot;
    }

    //-------------------------------------------------------------------------
    s32 Mesh::vertex_buffer_byte_stride() const
    {
      return m_vbd.byte_stride;
    }

    //-------------------------------------------------------------------------
    s32 Mesh::vertex_buffer_byte_size() const
    {
      return m_vbd.byte_size;
    }

    //-------------------------------------------------------------------------
    const ResourceSlot& Mesh::index_buffer_slot() const
    {
      return m_ibd.slot;
    }

    //-------------------------------------------------------------------------
    IndexBufferFormat Mesh::index_buffer_format() const
    {
      return m_ibd.format;
    }

    //-------------------------------------------------------------------------
    s32 Mesh::index_buffer_byte_size() const
    {
      return m_ibd.byte_size;
    }
  } // namespace renderer
} // namespace rex