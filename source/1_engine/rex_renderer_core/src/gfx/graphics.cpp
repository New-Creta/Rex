#include "rex_renderer_core/gfx/graphics.h"
#include "rex_renderer_core/gfx/gpu_engine.h"

#include "rex_renderer_core/rhi/rhi.h"
#include "rex_renderer_core/system/gpu_description.h"

#include "rex_std/bonus/utility.h"

namespace rex
{
  namespace gfx
  {
    // We can access it in the high level graphics api, but the gpu engine is owned by the rhi
    GpuEngine* g_gpu_engine;

    REX_STATIC_WARNING("Currently we have 2 graphics systems that are too similar, namely the gpu engine and the rhi.");
    REX_STATIC_WARNING("Define the responsibility of each of these or merge them together if possible, otherwise they'll just cause confusion");

    Error init(const renderer::OutputWindowUserData& userData)
    {
      // Initialize the gpu engine, it's responsible for the entire graphics pipeline.
      // The gpu engine gets created by the rhi as some of the objects wrapped by the gpu engine
      // are also needed by the rhi to create other objects.
      g_gpu_engine = rhi::init(userData);
      rsl::scopeguard shutdown_on_failure = []() { shutdown(); };

      // If we even fail to allocate the memory for the gpu engine, we exit immediately.
      // This is possible if we OOM or if any pre allocation checks fail in the rhi
      // and it has returns before even getting to the gpu engine initialization step
      if (!g_gpu_engine)
      {
        return Error("Failed to create GPU Engine");
      }

      // If the gpu engine itself failed to initialize, we also exit immediately.
      // This can be caused by many different reasons, best to look at the log for more info.
      if (!g_gpu_engine->init_successful())
      {
        return Error("Initialization of the gpu engine has failed");
      }

      // Make sure we release the scopeguard which would shutdown our graphics systems otherwise
      shutdown_on_failure.release();
      return Error::no_error();
    }
    void shutdown()
    {
      g_gpu_engine.reset();

      rhi::shutdown();
    }

    void new_frame()
    {
      g_gpu_engine->new_frame();
    }
    void begin_draw()
    {
      g_gpu_engine->begin_draw();
    }

    void end_draw()
    {
      g_gpu_engine->end_draw();
    }
    void present()
    {
      g_gpu_engine->present();
    }
    void end_frame()
    {
      g_gpu_engine->end_frame();
    }

    ScopedPoolObject<rhi::CopyContext> new_copy_ctx()
    {
      return g_gpu_engine->new_copy_ctx();
    }
    ScopedPoolObject<rhi::RenderContext> new_render_ctx()
    {
      return g_gpu_engine->new_render_ctx();
    }
  }
}