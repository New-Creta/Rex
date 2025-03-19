#pragma once

#include "rex_engine/diagnostics/error.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/engine/invalid_object.h"
#include "rex_engine/gfx/core/renderer_output_window_user_data.h"

#include "rex_engine/gfx/core/graphics_engine_type.h"

#include "rex_engine/gfx/system/command_allocator.h"
#include "rex_engine/gfx/system/command_queue.h"


#include "rex_engine/gfx/system/render_context.h"
#include "rex_engine/gfx/system/compute_context.h"
#include "rex_engine/gfx/system/copy_context.h"
#include "rex_engine/gfx/core/info.h"

#include "rex_engine/gfx/system/graphics_engine.h"
#include "rex_engine/gfx/rendering/renderer.h"

#include "rex_engine/gfx/resources/sampler_2d.h"
#include "rex_engine/gfx/resources/raster_state.h"

#include "rex_std/bonus/utility.h"

#include "rex_engine/engine/object_with_destruction_callback.h"

#include "rex_engine/gfx/system/gal.h"

// This is the interface of the main application with the graphics engine

 // #TODO: Remaining cleanup of development/Pokemon -> main merge. ID: GRAPHICS

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

    // Set the graphics abstraction layer object

    // Create and initialize the graphics engine, rendering is possible from here on out
    Error init(rsl::unique_ptr<GALInterface> galInterface, const OutputWindowUserData& userData);
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
    ObjectWithDestructionCallback<CopyContext> new_copy_ctx(PipelineState* pso = nullptr, rsl::string_view eventName = "");
    // Create a new context used for queueing rendering commands on the gpu
    ObjectWithDestructionCallback<RenderContext> new_render_ctx(PipelineState* pso = nullptr, rsl::string_view eventName = "");
    // Create a new context used for queueing compute commands on the gpu
    ObjectWithDestructionCallback<ComputeContext> new_compute_ctx(PipelineState* pso = nullptr, rsl::string_view eventName = "");

    // Return the current render target of the swapchain
    RenderTarget* swapchain_rt();
    // Return the width of the render target of the swapchain
    s32 swapchain_width();
    // Return the height of the render target of the swapchain
    s32 swapchain_height();

    // The following functions allow you to return a few common resource items
    enum class CommonRasterState
    {
      DefaultNoDepth,
      DefaultDepth
    };
    RasterStateDesc common_raster_state(CommonRasterState type);
    enum class CommonSampler
    {
      Default2D
    };
    Sampler2D* common_sampler(CommonSampler type);
  }
}