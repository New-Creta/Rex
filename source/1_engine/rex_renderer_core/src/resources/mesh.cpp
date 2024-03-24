#include "rex_renderer_core/resources/mesh.h"

#include "rex_renderer_core/diagnostics/log.h"

namespace rex
{
  namespace renderer
  {
    //-------------------------------------------------------------------------
    Mesh::Mesh(rsl::string_view name, VertexBufferDesc&& vbd, IndexBufferDesc&& ibd, ConstantBufferDesc&& cbd)
        : m_name(name)
        , m_vbd(rsl::move(vbd))
        , m_ibd(rsl::move(ibd))
        , m_cbd(rsl::move(cbd))
    {
    }

    //-------------------------------------------------------------------------
    void Mesh::add_submesh(rsl::string_view name, const Submesh& subMesh)
    {
      if(m_submesh_map.find(name) != rsl::cend(m_submesh_map))
      {
        REX_ERROR(LogRendererCore, "Submesh with name ({}) already added as a draw argument", name.data());
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
        REX_ERROR(LogRendererCore, "Submesh with name ({}) not found as a draw argument", name.data());
        return nullptr;
      }

      return &m_submesh_map.at(name);
    }

    const VertexBufferDesc* Mesh::vb() const
    {
      return &m_vbd;
    }
    const IndexBufferDesc* Mesh::ib() const
    {
      return &m_ibd;
    }
    const ConstantBufferDesc* Mesh::cb() const
    {
      return &m_cbd;
    }

    ////-------------------------------------------------------------------------
    //rsl::string_view Mesh::name() const
    //{
    //  return m_name;
    //}

    ////-------------------------------------------------------------------------
    //const ResourceSlot& Mesh::vertex_buffer_slot() const
    //{
    //  return m_vbd.slot;
    //}

    ////-------------------------------------------------------------------------
    //s32 Mesh::vertex_buffer_byte_stride() const
    //{
    //  return m_vbd.byte_stride;
    //}

    ////-------------------------------------------------------------------------
    //s32 Mesh::vertex_buffer_byte_size() const
    //{
    //  return m_vbd.total_size;
    //}

    ////-------------------------------------------------------------------------
    //const ResourceSlot& Mesh::index_buffer_slot() const
    //{
    //  return m_ibd.slot;
    //}

    ////-------------------------------------------------------------------------
    //IndexBufferFormat Mesh::index_buffer_format() const
    //{
    //  return m_ibd.format;
    //}

    ////-------------------------------------------------------------------------
    //s32 Mesh::index_buffer_byte_size() const
    //{
    //  return m_ibd.byte_size;
    //}
  } // namespace renderer
} // namespace rex