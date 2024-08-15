#pragma once

#include "rex_engine/diagnostics/error.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/engine/invalid_object.h"
#include "rex_renderer_core/gfx/renderer_output_window_user_data.h"

#include "rex_renderer_core/gfx/graphics_engine_type.h"

#include "rex_renderer_core/system/command_allocator.h"
#include "rex_renderer_core/system/command_queue.h"


#include "rex_renderer_core/gfx/render_context.h"
#include "rex_renderer_core/gfx/compute_context.h"
#include "rex_renderer_core/gfx/copy_context.h"
#include "rex_renderer_core/gfx/info.h"

#include "rex_renderer_core/system/graphics_engine.h"
#include "rex_renderer_core/rendering/renderer.h"

#include "rex_std/bonus/utility.h"

#include "rex_engine/pooling/scoped_pool_object.h"

// This is the interface of the main application with the graphics engine

namespace rex
{
  struct GpuDescription;

  namespace gfx
  {
    class PipelineState;

    namespace internal
    {
      // Internal call to add the renderer to the list of renderers
      void add_renderer(rsl::unique_ptr<Renderer> renderer);
    }

    // Log the basic info about the graphics hardware of the current machine
    void log_info();

    // Create and initialize the graphics engine, rendering is possible from here on out
    Error init(const OutputWindowUserData& userData);
    // Shutdown the graphics engine, no rendering support from here on out
    void shutdown();

    // Construct a new renderer and add it to the list of renderers we should call
    template <typename T, typename ... Args>
    T* add_renderer(Args&& ... args)
    {
      rsl::unique_ptr<T> renderer = rsl::make_unique<T>(rsl::forward<Args>(args)...);

      T* result = renderer.get();
      internal::add_renderer(rsl::move(renderer));

      return result;
    }

    // Render a single frame by going over all the renderers
    void render();

    // Create a new context used for copying data to the gpu
    ScopedPoolObject<CopyContext> new_copy_ctx(PipelineState* pso = nullptr, rsl::string_view eventName = "");
    // Create a new context used for queueing rendering commands on the gpu
    ScopedPoolObject<RenderContext> new_render_ctx(PipelineState* pso = nullptr, rsl::string_view eventName = "");
    // Create a new context used for queueing compute commands on the gpu
    ScopedPoolObject<ComputeContext> new_compute_ctx(PipelineState* pso = nullptr, rsl::string_view eventName = "");

    // Return the current render target of the swapchain
    RenderTarget* swapchain_rt();
    // Return the width of the render target of the swapchain
    s32 swapchain_width();
    // Return the height of the render target of the swapchain
    s32 swapchain_height();

  }
}