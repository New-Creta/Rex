#include "rex_renderer_core/gfx/graphics.h"
#include "rex_renderer_core/system/gpu_engine.h"

#include "rex_renderer_core/gfx/rhi.h"
#include "rex_renderer_core/system/gpu_description.h"

#include "rex_std/bonus/utility.h"

namespace rex
{
  namespace gfx
  {
    // NOTE: To keep in mind when abstracting constant buffer data upload
    //
    // Shaders take multiple levels of data
    // 1. Per View Data
    // This data is shared by all materials used in a view. Should contain all information like camera matrices, light directions, time, ..
    // 
    // 2. Per Pass Data
    // This data is shared by all materials within the same pass. This is only relevant once we have multiple render passes
    // 
    // 3. Per material data
    // This data contains information per material like textures
    // 
    // 4. Per instance data
    // Contains data that's separate per instance. For example a world view projection matrix
    //





    DEFINE_LOG_CATEGORY(LogGraphics);

    // We can access it in the high level graphics api, but the gpu engine is owned by the rhi
    GpuEngine* g_gpu_engine;
    rsl::vector<rsl::unique_ptr<Renderer>> g_renderers;

    void log_info()
    {
      auto& gfx_info = rhi::info();
      REX_INFO(LogGraphics, "Renderer Info - Adaptor: {}", gfx_info.adaptor);
      REX_INFO(LogGraphics, "Renderer Info - Vendor: {}", gfx_info.vendor);
      REX_INFO(LogGraphics, "Renderer Info - API: {}", gfx_info.api);
      REX_INFO(LogGraphics, "Renderer Info - API Version: {}", gfx_info.api_version);
      REX_INFO(LogGraphics, "Renderer Info - Shader Version: {}", gfx_info.shader_version);
      REX_INFO(LogGraphics, "Renderer Info - Driver Version: {}", gfx_info.driver_version);
    }

    Error init(const OutputWindowUserData& userData)
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

      // Make sure we release the scopeguard which would shutdown our graphics systems otherwise
      shutdown_on_failure.release();
      return Error::no_error();
    }
    void shutdown()
    {
      g_renderers.clear();

      rhi::shutdown();
    }

    void add_renderer(rsl::unique_ptr<Renderer> renderer)
    {
      g_renderers.emplace_back(rsl::move(renderer));
    }

    void render()
    {
      // The render loop is very simple
      // - Prepare a new frame
      // - Loop over all the renderers
      // - Finalize the frame
      // - Present

      // Prepare all resources that are needed for a new frame
      g_gpu_engine->new_frame();
      
      // Loop over all renderers and call into them
      // Performing all required gpu operations needed on resources those renderers need
      // The resources are owned by the renderers, they're responsible for creating and destroying them
      // However in the backend, the gpu manager controls when these resources actually get deallocated
      for (auto& renderer : g_renderers)
      {
        renderer->render();
      }

      // Finalize the frame and bringing it ready for presentation
      g_gpu_engine->end_frame();

      // Present the last rendered frame to the screen
      g_gpu_engine->present();
    }

    // Create a new copy context that automatically gets added back to the pool when it goes out of scope
    ScopedPoolObject<CopyContext> new_copy_ctx(PipelineState* pso, rsl::string_view eventName)
    {
      return g_gpu_engine->new_copy_ctx(pso, eventName);
    }
    // Create a new render context that automatically gets added back to the pool when it goes out of scope
    ScopedPoolObject<RenderContext> new_render_ctx(PipelineState* pso, rsl::string_view eventName)
    {
      return g_gpu_engine->new_render_ctx(pso, eventName);
    }
    // Create a new compute context that automatically gets added back to the pool when it goes out of scope
    ScopedPoolObject<ComputeContext> new_compute_ctx(PipelineState* pso, rsl::string_view eventName)
    {
      return g_gpu_engine->new_compute_ctx(pso, eventName);
    }
  }
}