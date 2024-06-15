#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/gfx/primitive_topology.h"
#include "rex_renderer_core/resources/mesh.h"
#include "rex_renderer_core/resources/vertex_buffer.h"
#include "rex_renderer_core/resources/index_buffer.h"
#include "rex_renderer_core/resources/constant_buffer.h"

#include <glm/glm.hpp>

namespace rex
{
  namespace gfx
  {
    // Descriptor holding all the data needed to create a render item
    struct RenderItemDesc
    {
      VertexBufferDesc vb_desc;
      IndexBufferDesc ib_desc;
      ConstantBufferDesc cb_desc;
      PrimitiveTopology topology;
    };

    // A render item is the renderer's representation of something in the world/scene.
    // It holds its vertex buffer, index buffer and any material settings needed to render this object.
    class RenderItem
    {
    public:
      // Construct the render item object
      RenderItem(const ResourceSlot& vb, const ResourceSlot& ib, const ResourceSlot& cb, PrimitiveTopology topology, s32 indexCount, s32 baseVertexLocation = 0);

      // return the slot of the vertex buffer
      ResourceSlot vb() const;
      // return the slot of the index buffer
      ResourceSlot ib() const;
      // return the slot of the constantbuffer
      ResourceSlot cb() const;
      // return the primitive topology
      PrimitiveTopology primtive_topology() const;

      // return the number of indices
      s32 index_count() const;
      // return the start index
      s32 start_index() const;
      // return the base vertex location.
      // this value gets automatically added by the gpu to any index before drawing
      // this is useful in case in case you're working with a big vertex and index buffer
      // holding multiple objects
      s32 base_vertex_loc() const;

    private:
      ResourceSlot m_vb;
      ResourceSlot m_ib;
      ResourceSlot m_cb;
      PrimitiveTopology m_topology;

      s32 m_index_count {};
      s32 m_start_index_location {};
      s32 m_base_vertex_location {};
    };
  } // namespace gfx
} // namespace rex