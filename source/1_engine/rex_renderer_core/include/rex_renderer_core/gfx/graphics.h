#pragma once

#include "rex_engine/diagnostics/error.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/engine/invalid_object.h"
#include "rex_renderer_core/rendering/renderer_output_window_user_data.h"

#include "rex_renderer_core/rhi/command_type.h"

#include "rex_renderer_core/system/command_allocator.h"
#include "rex_renderer_core/system/command_queue.h"
#include "rex_renderer_core/system/command_list.h"

#include "rex_renderer_core/rhi/render_context.h"
#include "rex_renderer_core/rhi/compute_context.h"
#include "rex_renderer_core/rhi/copy_context.h"

#include "rex_renderer_core/gfx/graphics_engine.h"

#include "rex_std/bonus/utility.h"

#include "rex_engine/pooling/scoped_pool_object.h"

// The graphics context acts as the interface for renderers to speak with the graphics api
// eg. for DirectX it holds the following objects
// The DirectX Device
//   This is the objects needed to create gpu resources
// The Graphics Engine
//   This is reponsible for creating various types of sub engines (eg. copy engine, render engine, compute engine)
//
// The Compute Engine
//
// The Commandlist Manager
//   This object is responsible of creating command lists
//   Pooling them when they're no longer used
//   It holds an internal command allocator pool to make sure
//   Command lists get assigned an allocator that's not currently used
//

namespace rex
{
  struct GpuDescription;

  namespace gfx
  {
    // Create and initialize the graphics engine, rendering is possible from here on out
    Error init(const renderer::OutputWindowUserData& userData);
    // Shutdown the graphics engine, no rendering support from here on out
    void shutdown();

    void new_frame();
    void begin_draw();

    void end_draw();
    void present();
    void end_frame();

    ScopedPoolObject<rhi::CopyContext> new_copy_ctx();
    ScopedPoolObject<rhi::RenderContext> new_render_ctx();
  }
}