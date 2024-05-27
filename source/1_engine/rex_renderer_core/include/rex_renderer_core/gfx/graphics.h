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
  namespace gfx
  {
    // Create and initialize the graphics engine, rendering is possible from here on out
    Error init(const renderer::OutputWindowUserData& userData);
    // Shutdown the graphics engine, no rendering support from here on out
    void shutdown();

    // The GPU Engine is the main system driving all other systems that deal with the gpu.
    // It encapsulates all sub engines (copy, compute, render).
    class GpuEngine
    {
    public:
      GpuEngine();

      // Get the command queue of the specified type and check if its internal fence
      // is higher or equal to the given value
      bool is_fence_complete(rhi::CommandType cmdType, s32 fenceValue) const;
      // Get the command queue of the specified type and wait for its internal fence
      // to be higher or equal to the given value
      void wait_for_fence(rhi::CommandType cmdType, s32 fenceValue);
      // Wait for all command queue to have finished executing their current queued commands
      void wait_for_gpu();

      // Create a new context which is used for copying resources from or to the gpu
      ScopedPoolObject<rhi::CopyContext> new_copy_ctx();
      // Create a new context which is used for rendering to render targets
      ScopedPoolObject<rhi::RenderContext> new_render_ctx();
      // Create a new context which is used for computing data on the gpu
      ScopedPoolObject<rhi::ComputeContext> new_compute_ctx();

    private:
      rsl::unique_ptr<rhi::RenderEngine> m_render_engine;
      rsl::unique_ptr<rhi::ComputeEngine> m_compute_engine;
      rsl::unique_ptr<rhi::CopyEngine> m_copy_engine;
    };

    rhi::RenderContext& begin_render_context(rsl::string_view name)
    {
      // Get a graphics context from the gpu engine (allocating a new one if necessary)
      // ..
      // Initialize it so it is prepared for rendering (reset the commandlist, assign desc heaps, ..)
      // ..
      // Return the context to the user
      // ..
    }

    ScopedPoolObject<rhi::CopyContext> new_copy_ctx();
    ScopedPoolObject<rhi::RenderContext> new_render_ctx();
  }
}