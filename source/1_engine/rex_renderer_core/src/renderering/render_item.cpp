#include "rex_renderer_core/rendering/render_item.h"

namespace rex
{
  namespace renderer
  {
    RenderItem::RenderItem(const rhi::ResourceSlot& vb, const rhi::ResourceSlot& ib, const rhi::ResourceSlot& cb, PrimitiveTopology topology, s32 indexCount, s32 baseVertexLocation)
      : m_vb(vb)
      , m_ib(ib)
      , m_cb(cb)
      , m_topology(topology)
    {
      m_start_index_location = 0;
      m_base_vertex_location = baseVertexLocation;
      m_index_count = indexCount;
    }

    rhi::ResourceSlot RenderItem::vb() const
    {
      return m_vb;
    }
    rhi::ResourceSlot RenderItem::ib() const
    {
      return m_ib;
    }
    rhi::ResourceSlot RenderItem::cb() const
    {
      return m_cb;
    }
    PrimitiveTopology RenderItem::primtive_topology() const
    {
      return m_topology;
    }

    s32 RenderItem::index_count() const
    {
      return m_index_count;
    }
    s32 RenderItem::start_index() const
    {
      return m_start_index_location;
    }
    s32 RenderItem::base_vertex_loc() const
    {
      return m_base_vertex_location;
    }

  }
}