#include "rex_renderer_core/gfx/gpu_engine.h"

#include "rex_renderer_core/resources/render_target.h"
#include "rex_renderer_core/resources/texture_2d.h"

namespace rex
{
  namespace gfx
  {
    // Make sure to not try and initialize any gpu resources in the constructor.
    // The derived class of the gpu engine is responsible for making sure the gpu is ready.
    GpuEngine::GpuEngine(const renderer::OutputWindowUserData& userData)
      : m_render_engine(rsl::make_unique<RenderEngine>())
      , m_compute_engine(rsl::make_unique<ComputeEngine>())
      , m_copy_engine(rsl::make_unique<CopyEngine>())
      , m_swapchain(rhi::create_swapchain(m_render_engine->command_queue(), userData.max_frames_in_flight, userData.primary_display_handle))
      , m_init_successfully(true)
    {
      init_swapchain_render_targets();
    }

    bool GpuEngine::init_successful() const
    {
      return m_init_successfully;
    }

    void GpuEngine::new_frame()
    {
      // Nothing to implement
    }
    void GpuEngine::begin_draw()
    {
      // Nothing to implement
    }

    void GpuEngine::end_draw()
    {
      // Nothing to implement
    }
    void GpuEngine::present()
    {
      m_swapchain->present();
    }
    void GpuEngine::end_frame()
    {
      // Nothing to implement
    }

    void GpuEngine::init_swapchain_render_targets()
    {
      for (s32 i = 0; i < m_swapchain->num_buffers(); ++i)
      {
        rhi::Texture2D* texture = m_swapchain->buffer(i);
        rsl::unique_ptr<rhi::RenderTarget> render_target = rhi::create_render_target(texture);
        m_swapchain_render_targets.emplace_back(rsl::move(render_target));
      }
    }





    // Get the command queue of the specified type and check if its internal fence
    // is higher or equal to the given value
    bool GpuEngine::is_fence_complete(rhi::CommandType cmdType, s32 fenceValue) const
    {
      REX_ASSERT("Function not yet implemented");
      return false;
    }
    // Get the command queue of the specified type and wait for its internal fence
    // to be higher or equal to the given value
    void GpuEngine::wait_for_fence(rhi::CommandType cmdType, s32 fenceValue)
    {
      REX_ASSERT("Function not yet implemented");

    }
    // Wait for all command queue to have finished executing their current queued commands
    void GpuEngine::wait_for_gpu()
    {
      REX_ASSERT("Function not yet implemented");

    }

    s32 GpuEngine::calc_gpu_score(const GpuDescription& gpu)
    {
      return gpu.dedicated_video_memory.size_in_bytes();
    }

    void GpuEngine::init_failed()
    {
      m_init_successfully = false;
    }

    ScopedPoolObject<rhi::CopyContext> GpuEngine::new_copy_ctx()
    {
      return m_copy_engine->new_context();
    }

    ScopedPoolObject<rhi::RenderContext> GpuEngine::new_render_ctx()
    {
      return m_render_engine->new_context();
    }

    ScopedPoolObject<rhi::ComputeContext> GpuEngine::new_compute_ctx()
    {
      return m_compute_engine->new_context();
    }

  }
}