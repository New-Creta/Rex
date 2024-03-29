#include "rex_renderer_core/system/renderer.h"

#include "rex_engine/engine/defines.h"
#include "rex_engine/containers/ring_buffer.h"
#include "rex_renderer_core/rendering/default_depth_info.h"
#include "rex_renderer_core/rendering/default_targets_info.h"
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
  namespace globals
  {
    DefaultDepthInfo g_default_depth_info;     // NOLINT(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)
    DefaultTargetsInfo g_default_targets_info; // NOLINT(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)

    //-------------------------------------------------------------------------
    const DefaultDepthInfo& default_depth_info()
    {
      return g_default_depth_info;
    }

    //-------------------------------------------------------------------------
    const DefaultTargetsInfo& default_targets_info()
    {
      return g_default_targets_info;
    }

  } // namespace globals

  namespace renderer
  {
    using CommandAllocator = rsl::stack_allocator;

    //-------------------------------------------------------------------------
    bool initialize(const OutputWindowUserData& userData, s32 maxCommands, s32 maxFrameResources)
    {
      return backend::initialize(userData);
    }

    void set_shader(const rhi::ResourceSlot& slot)
    {
      rhi::set_shader(slot);
    }
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

    RenderItem* add_mesh(const Mesh* mesh)
    {
      // We want to add a new mesh to the renderer so it gets rendered the next frame
      // Questions is how do we do that and what data do we need to set to do this?

      // In the future this will be a model with materials etc, but for now a mesh is just
      // a wrapper around a vertex buffer, index buffer, topology and a constant buffer
      // So this sounds like the data we need to set

      RenderItemDesc render_item{};

      render_item.vb_desc = mesh->vb();
      render_item.ib_desc = mesh->ib();
      render_item.cb_desc = mesh->cb();
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
    bool end_frame(FlushCommands flush)
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