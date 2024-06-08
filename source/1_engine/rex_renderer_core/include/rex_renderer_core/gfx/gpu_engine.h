#pragma once

#include "rex_renderer_core/rendering/renderer_output_window_user_data.h"
#include "rex_engine/pooling/scoped_pool_object.h"
#include "rex_renderer_core/rhi/command_type.h"
#include "rex_renderer_core/rhi/copy_context.h"
#include "rex_renderer_core/rhi/compute_context.h"
#include "rex_renderer_core/rhi/render_context.h"
#include "rex_renderer_core/system/command_queue.h"
#include "rex_renderer_core/system/gpu_description.h"
#include "rex_renderer_core/gfx/render_engine.h"
#include "rex_renderer_core/gfx/copy_engine.h"
#include "rex_renderer_core/gfx/compute_engine.h"
#include "rex_renderer_core/resources/clear_state.h"

namespace rex
{
  namespace gfx
  {
    // The GPU Engine is the main system driving all other systems that deal with the gpu.
    // It encapsulates all sub engines (copy, compute, render).
    class GpuEngine
    {
    public:
      GpuEngine(rsl::unique_ptr<RenderEngine> renderEngine, rsl::unique_ptr<ComputeEngine> computeEngine, rsl::unique_ptr<CopyEngine> copyEngine, const renderer::OutputWindowUserData& userData);
      GpuEngine(const GpuEngine&) = delete;
      GpuEngine(GpuEngine&&) = delete;

      virtual ~GpuEngine() = default;

      GpuEngine& operator=(const GpuEngine&) = delete;
      GpuEngine& operator=(GpuEngine&&) = delete;

      // Because some objects have a dependency on the gpu engine itself
      // We need to wait for the gpu engine to be constructed, only then 
      // can we initialize the rest of the objects
      void init();

      // Prepare a new frame by incrementing the frame index and clearing the backbuffer
      void new_frame();
      // Present the new frame to the main window
      void present();
      // Finish off the last frame
      void end_frame();

      // Create a new context which is used for copying resources from or to the gpu
      ScopedPoolObject<rhi::CopyContext> new_copy_ctx();
      // Create a new context which is used for rendering to render targets
      ScopedPoolObject<rhi::RenderContext> new_render_ctx();
      // Create a new context which is used for computing data on the gpu
      ScopedPoolObject<rhi::ComputeContext> new_compute_ctx();

      // Return the render target pointing to the current backbuffer of the swapchain
      rhi::RenderTarget* render_target();

    protected:
      // Initialize the resource heap which keeps track of all gpu resources
      virtual void init_resource_heap() = 0;
      // Initialize the descriptor heaps which keep track of all descriptors to various resources
      virtual void init_descriptor_heaps() = 0;

    private:
      // Initialize the clear state which is used to clear the backbuffer with
      void init_clear_state();
      // Initialize the swapchain which is used for presenting to the main window
      void init_swapchain();
      // Initialize the sub engine, bringing them up and ready, to be used in the graphics pipeline
      void init_sub_engines();
      // Initialize imgui so it can create its own windows if necessary
      void init_imgui();

    private:
      rsl::unique_ptr<RenderEngine> m_render_engine;    // The render engine is the high level graphics engine responsible for queueing render commands
      rsl::unique_ptr<ComputeEngine> m_compute_engine;  // The render engine is the high level graphics engine responsible for queueing compute commands
      rsl::unique_ptr<CopyEngine> m_copy_engine;        // The render engine is the high level graphics engine responsible for queueing copy commands
      rsl::unique_ptr<rhi::Swapchain> m_swapchain;      // The swapchain is responsible for swapping the backbuffer with the front buffer
      rsl::vector<rsl::unique_ptr<rhi::RenderTarget>> m_swapchain_render_targets; // The render targets that point to the buffers of the swapchain
      rsl::unique_ptr<rhi::ClearStateResource> m_clear_state_resource; // The clear state resource that's used to clear the backbuffer with
      s32 m_max_frames_in_flight;                       // The maximum number of we can have in flight for rendering.
      void* m_primary_display_handle;                   // The display handle to render to (HWND on Windows)
    };

  }
}