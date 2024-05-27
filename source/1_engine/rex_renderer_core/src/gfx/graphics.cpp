#include "rex_renderer_core/gfx/graphics.h"

#include "rex_renderer_core/rhi/rhi.h"

namespace rex
{
  namespace gfx
  {
    rsl::unique_ptr<GpuEngine> g_gpu_engine;

    Error init(const renderer::OutputWindowUserData& userData)
    {
      s32 max_frames_in_flight = 3;
      Error err = rhi::init(userData, max_frames_in_flight);

      if (err)
      {
        return err;
      }

      g_gpu_engine = rsl::make_unique<GpuEngine>();
      err = g_gpu_engine->get_last_error();

      if (err)
      {
        return err;
      }

      return err;
    }
    void shutdown()
    {
      g_gpu_engine.reset();

      rhi::shutdown();
    }

    ScopedPoolObject<CopyContext> new_copy_ctx()
    {
      return g_gpu_engine->new_copy_ctx();
    }
    ScopedPoolObject<rhi::RenderContext> new_render_ctx()
    {
      return g_gpu_engine->new_render_ctx();
    }
  }
}