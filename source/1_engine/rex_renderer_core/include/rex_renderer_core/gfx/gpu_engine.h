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

      void post_init();

      // Did the gpu engine initialize successfully?
      bool init_successful() const;

      void new_frame();
      void begin_draw();

      void end_draw();
      void present();
      void end_frame();

      // Create a new context which is used for copying resources from or to the gpu
      ScopedPoolObject<rhi::CopyContext> new_copy_ctx();
      // Create a new context which is used for rendering to render targets
      ScopedPoolObject<rhi::RenderContext> new_render_ctx();
      // Create a new context which is used for computing data on the gpu
      ScopedPoolObject<rhi::ComputeContext> new_compute_ctx();



      // Get the command queue of the specified type and check if its internal fence
      // is higher or equal to the given value
      bool is_fence_complete(rhi::CommandType cmdType, s32 fenceValue) const;
      // Get the command queue of the specified type and wait for its internal fence
      // to be higher or equal to the given value
      void wait_for_fence(rhi::CommandType cmdType, s32 fenceValue);
      // Wait for all command queue to have finished executing their current queued commands
      void wait_for_gpu();

    protected:
      s32 calc_gpu_score(const GpuDescription& gpu);
      void init_failed();

    private:
      void init_swapchain_render_targets();

    private:
      rsl::unique_ptr<RenderEngine> m_render_engine;
      rsl::unique_ptr<ComputeEngine> m_compute_engine;
      rsl::unique_ptr<CopyEngine> m_copy_engine;
      rsl::unique_ptr<rhi::Swapchain> m_swapchain;
      rsl::vector<rsl::unique_ptr<rhi::RenderTarget>> m_swapchain_render_targets;
      s32 m_max_frames_in_flight;
      void* m_primary_display_handle;
      bool m_init_successfully;
    };

  }
}