#pragma once

#include "rex_renderer_core/gfx/renderer_output_window_user_data.h"
#include "rex_engine/pooling/scoped_pool_object.h"
#include "rex_renderer_core/gfx/graphics_engine_type.h"
#include "rex_renderer_core/gfx/copy_context.h"
#include "rex_renderer_core/gfx/compute_context.h"
#include "rex_renderer_core/gfx/render_context.h"
#include "rex_renderer_core/system/command_queue.h"
#include "rex_renderer_core/system/gpu_description.h"
#include "rex_renderer_core/system/render_engine.h"
#include "rex_renderer_core/system/copy_engine.h"
#include "rex_renderer_core/system/compute_engine.h"
#include "rex_renderer_core/resources/clear_state.h"
#include "rex_renderer_core/gfx/descriptor_heap_type.h"
#include "rex_renderer_core/system/descriptor_heap.h"
#include "rex_renderer_core/system/resource_state_tracker.h"

#include "rex_std/unordered_map.h"

namespace rex
{
  namespace gfx
  {
    // The GPU Engine is the main system driving all other systems that deal with the gpu.
    // It encapsulates all sub engines (copy, compute, render).
    class GpuEngine
    {
    public:
      GpuEngine(const OutputWindowUserData& userData);
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
      ScopedPoolObject<CopyContext> new_copy_ctx();
      // Create a new context which is used for rendering to render targets
      ScopedPoolObject<RenderContext> new_render_ctx();
      // Create a new context which is used for computing data on the gpu
      ScopedPoolObject<ComputeContext> new_compute_ctx();

      // Return the render target pointing to the current backbuffer of the swapchain
      RenderTarget* current_backbuffer_rt();

    protected:
      // Initialize the various sub engines
      virtual rsl::unique_ptr<RenderEngine> init_render_engine(ResourceStateTracker* resourceStateTracker) = 0;
      virtual rsl::unique_ptr<CopyEngine> init_copy_engine(ResourceStateTracker* resourceStateTracker) = 0;
      virtual rsl::unique_ptr<ComputeEngine> init_compute_engine(ResourceStateTracker* resourceStateTracker) = 0;

      // Initialize the resource heap which allocates all gpu resources
      virtual void init_resource_heap() = 0;
      // Allocate a new descriptor heap of a given type
      virtual rsl::unique_ptr<DescriptorHeap> allocate_desc_heap(DescriptorHeapType descHeapType) = 0;

      // Returns a specific descriptor heap based on type
      DescriptorHeap* desc_heap(DescriptorHeapType descHeapType);

    private:
      // Initialize the clear state which is used to clear the backbuffer with
      void init_clear_state();
      // Initialize the swapchain which is used for presenting to the main window
      void init_swapchain();
      // Initialize the sub engine, bringing them up and ready, to be used in the graphics pipeline
      void init_sub_engines();
      // Initialize imgui so it can create its own windows if necessary
      void init_imgui();
      // Initialize the descriptor heaps which keep track of all descriptors to various resources
      void init_desc_heaps();
      void init_desc_heap(DescriptorHeapType descHeapType);

    private:
      rsl::unique_ptr<Swapchain> m_swapchain;      // The swapchain is responsible for swapping the backbuffer with the front buffer
      rsl::unique_ptr<RenderEngine> m_render_engine;    // The render engine is the high level graphics engine responsible for queueing render commands
      rsl::unique_ptr<ComputeEngine> m_compute_engine;  // The render engine is the high level graphics engine responsible for queueing compute commands
      rsl::unique_ptr<CopyEngine> m_copy_engine;        // The render engine is the high level graphics engine responsible for queueing copy commands
      rsl::unique_ptr<ClearState> m_clear_state_resource; // The clear state resource that's used to clear the backbuffer with
      s32 m_max_frames_in_flight;                       // The maximum number of we can have in flight for rendering.
      void* m_primary_display_handle;                   // The display handle to render to (HWND on Windows)
      s32 m_frame_idx;                                  // The current frame index
      rsl::unordered_map<DescriptorHeapType, rsl::unique_ptr<DescriptorHeap>> m_descriptor_heap_pool; // Pool of descriptor heaps per type
      ResourceStateTracker m_resource_state_tracker; // The global tracker of resource states
      
    };

  }
}