#pragma once

#include "rex_engine/diagnostics/error.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/engine/invalid_object.h"
#include "rex_renderer_core/rendering/renderer_output_window_user_data.h"

#include "rex_renderer_core/rhi/graphics_engine_type.h"

#include "rex_renderer_core/system/command_allocator.h"
#include "rex_renderer_core/system/command_queue.h"


#include "rex_renderer_core/rhi/render_context.h"
#include "rex_renderer_core/rhi/compute_context.h"
#include "rex_renderer_core/rhi/copy_context.h"

#include "rex_renderer_core/gfx/graphics_engine.h"
#include "rex_renderer_core/rendering/renderer.h"

#include "rex_std/bonus/utility.h"

#include "rex_engine/pooling/scoped_pool_object.h"

// This is the interface of the main application with the graphics engine

namespace rex
{
  struct GpuDescription;

  namespace gfx
  {
    // Create and initialize the graphics engine, rendering is possible from here on out
    Error init(const renderer::OutputWindowUserData& userData);
    // Shutdown the graphics engine, no rendering support from here on out
    void shutdown();

    // Add a renderer to the queue.
    // All renderers get processed when render() is called
    void add_renderer(rsl::unique_ptr<Renderer> renderer);

    // Render a single frame by going over all the renderers
    void render();

    // Present the last renderer frame to the main window
    void present();

    // Create a new context used for copying data to the gpu
    ScopedPoolObject<rhi::CopyContext> new_copy_ctx();
    // Create a new context used for queueing rendering commands on the gpu
    ScopedPoolObject<rhi::RenderContext> new_render_ctx();
    // Create a new context used for queueing compute commands on the gpu
    ScopedPoolObject<rhi::ComputeContext> new_compute_ctx();
  }
}