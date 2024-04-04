#include "rex_renderer_core/system/renderer.h"

#include "rex_engine/engine/defines.h"
#include "rex_engine/containers/ring_buffer.h"
#include "rex_renderer_core/rendering/depth_info.h"
#include "rex_renderer_core/rendering/index_buffer_format.h"
#include "rex_renderer_core/diagnostics/log.h"
#include "rex_renderer_core/rendering/primitive_topology.h"
#include "rex_renderer_core/system/renderer_backend.h"
#include "rex_renderer_core/rendering/scissor_rect.h"
#include "rex_renderer_core/shaders/shader_platform.h"
#include "rex_renderer_core/shaders/shader_type.h"
#include "rex_renderer_core/rendering/viewport.h"
#include "rex_std/bonus/memory/stack_allocator.h"
#include "rex_std/bonus/utility/enum_reflection.h"

#include "rex_renderer_core/resources/mesh.h"
#include "rex_renderer_core/rendering/render_item.h"

#include "rex_renderer_core/system/rhi.h"

namespace rex
{
  namespace renderer
  {
    using CommandAllocator = rsl::stack_allocator;

    //-------------------------------------------------------------------------
    bool initialize(const OutputWindowUserData& userData)
    {
      return backend::initialize(userData);
    }

    //-------------------------------------------------------------------------
    void set_shader(const rhi::ResourceSlot& slot)
    {
      rhi::set_shader(slot);
    }
    //-------------------------------------------------------------------------
    void set_pso(const rhi::ResourceSlot& slot)
    {
      rhi::set_pso(slot);
    }

    //-------------------------------------------------------------------------
    void render()
    {
      backend::render();
    }

    //-------------------------------------------------------------------------
    void shutdown()
    {
      backend::shutdown();
    }

    RenderItem* add_mesh(const Mesh* mesh, const Submesh& subMesh, rex::memory::BlobView cbView)
    {
      // We want to add a new mesh to the renderer so it gets rendered the next frame
      // Questions is how do we do that and what data do we need to set to do this?

      // In the future this will be a model with materials etc, but for now a mesh is just
      // a wrapper around a vertex buffer, index buffer, topology and a constant buffer
      // So this sounds like the data we need to set

      RenderItemDesc render_item{};

      const rsl::byte* start_ib = mesh->ib()->blob_view.data() + (subMesh.start_index_location * index_format_size(mesh->ib()->format));
      const s32 size_ib = subMesh.index_count * index_format_size(mesh->ib()->format);

      rhi::ConstantBufferDesc cb_desc{};
      cb_desc.blob_view = cbView;

      render_item.vb_desc = *mesh->vb();
      render_item.ib_desc = *mesh->ib();
      render_item.ib_desc.blob_view = rex::memory::BlobView(start_ib, rsl::memory_size(size_ib));
      render_item.ib_desc.index_count = subMesh.index_count;
      render_item.ib_desc.base_vertex_loc = subMesh.base_vertex_location;
      render_item.cb_desc = cb_desc;
      render_item.topology = PrimitiveTopology::TRIANGLELIST;

      // Now that we've set the data, we need to let the backend know that we have a new render item
      // to draw next frame. The backend is a platform specific call that has optimised logic
      // to add render items to the renderer
      return backend::add_render_item(render_item);
    }


    //-------------------------------------------------------------------------
    bool prepare_user_initialization()
    {
      rhi::prepare_user_initialization();

      return true;
    }

    //-------------------------------------------------------------------------
    bool finish_user_initialization()
    {
      rhi::finish_user_initialization();

      return true;
    }

    void set_viewport(const Viewport& viewport)
    {
      backend::set_viewport(viewport);
    }
    void set_scissor_rect(const ScissorRect& rect)
    {
      backend::set_scissor_rect(rect);
    }

    //-------------------------------------------------------------------------
    bool new_frame()
    {
      return renderer::backend::new_frame();
    }

    //-------------------------------------------------------------------------
    bool end_frame()
    {
      return backend::end_frame();
    }

    //-------------------------------------------------------------------------
    bool begin_draw()
    {
      return backend::begin_draw();
    }

    //-------------------------------------------------------------------------
    bool end_draw()
    {
      return backend::end_draw();
    }

    //-------------------------------------------------------------------------
    bool present()
    {
      return backend::present();
    }

  } // namespace renderer
} // namespace rex