#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/rendering/primitive_topology.h"
#include "rex_renderer_core/resources/constant_buffer.h"
#include "rex_renderer_core/resources/index_buffer.h"
#include "rex_renderer_core/resources/mesh.h"
#include "rex_renderer_core/resources/vertex_buffer.h"

#include <glm/glm.hpp>

namespace rex
{
  namespace renderer
  {
    struct RenderItemDesc
    {
      rhi::VertexBufferDesc vb_desc{};
      rhi::IndexBufferDesc ib_desc{};
      rhi::ConstantBufferDesc cb_desc{};
      PrimitiveTopology topology{};
    };

    struct RenderItem
    {
    public:
      RenderItem(const rhi::ResourceSlot& vb, const rhi::ResourceSlot& ib, const rhi::ResourceSlot& cb, PrimitiveTopology topology, s32 indexCount, s32 baseVertexLocation = 0)
          : m_vb(vb)
          , m_ib(ib)
          , m_cb(cb)
          , m_topology(topology)
          , m_start_index_location(0)
          , m_base_vertex_location(baseVertexLocation)
          , m_index_count(indexCount)
      {
      }

      rhi::ResourceSlot vb() const
      {
        return m_vb;
      }
      rhi::ResourceSlot ib() const
      {
        return m_ib;
      }
      rhi::ResourceSlot cb() const
      {
        return m_cb;
      }
      PrimitiveTopology primtive_topology() const
      {
        return m_topology;
      }

      s32 index_count() const
      {
        return m_index_count;
      }
      s32 start_index() const
      {
        return m_start_index_location;
      }
      s32 base_vertex_loc() const
      {
        return m_base_vertex_location;
      }

    private:
      rhi::ResourceSlot m_vb;
      rhi::ResourceSlot m_ib;
      rhi::ResourceSlot m_cb;
      PrimitiveTopology m_topology;

      s32 m_index_count {};
      s32 m_start_index_location {};
      s32 m_base_vertex_location {};
    };
  } // namespace renderer
} // namespace rex