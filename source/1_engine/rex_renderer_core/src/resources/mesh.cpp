#include "rex_renderer_core/resources/mesh.h"

#include "rex_renderer_core/diagnostics/log.h"

namespace rex
{
  namespace renderer
  {
    //-------------------------------------------------------------------------
    Mesh::Mesh(rsl::string_view name, rhi::VertexBufferDesc vbd, rhi::IndexBufferDesc ibd)
        : m_name(name)
        , m_vbd(rsl::move(vbd))
        , m_ibd(rsl::move(ibd))
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

    const rhi::VertexBufferDesc* Mesh::vb() const
    {
      return &m_vbd;
    }
    const rhi::IndexBufferDesc* Mesh::ib() const
    {
      return &m_ibd;
    }

  } // namespace renderer
} // namespace rex