#include "rex_renderer_core/gfx/gpu_engine.h"

#include "rex_renderer_core/resources/render_target.h"
#include "rex_renderer_core/resources/texture_2d.h"

namespace rex
{
  namespace gfx
  {
    // Make sure to not try and initialize any gpu resources in the constructor.
    // The derived class of the gpu engine is responsible for making sure the gpu is ready.
    GpuEngine::GpuEngine(rsl::unique_ptr<RenderEngine> renderEngine, rsl::unique_ptr<ComputeEngine> computeEngine, rsl::unique_ptr<CopyEngine> copyEngine, const renderer::OutputWindowUserData& userData)
      : m_render_engine(rsl::move(renderEngine))
      , m_compute_engine(rsl::move(computeEngine))
      , m_copy_engine(rsl::move(copyEngine))
      , m_swapchain()
      , m_max_frames_in_flight(userData.max_frames_in_flight)
      , m_primary_display_handle(userData.primary_display_handle)
      , m_init_successfully(true)
    {
      rhi::ClearStateDesc desc{};
      desc.rgba = rsl::colors::LightSteelBlue;
      desc.flags.add_state(renderer::ClearBits::ClearColorBuffer);
      m_clear_state_resource = rsl::make_unique<rhi::ClearStateResource>(desc);
    }

    void GpuEngine::post_init()
    {
      m_swapchain = rhi::create_swapchain(m_render_engine->command_queue(), m_max_frames_in_flight, m_primary_display_handle);
      init_swapchain_render_targets();

      m_copy_engine->post_init();
    }

    bool GpuEngine::init_successful() const
    {
      return m_init_successfully;
    }

    void GpuEngine::new_frame()
    {
      auto render_ctx = new_render_ctx();
      render_ctx->clear_render_target(render_target(), m_clear_state_resource.get());
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
      {
        auto render_ctx = new_render_ctx();
        render_ctx->transition_buffer(m_swapchain->buffer(m_swapchain->current_buffer_idx()), rhi::ResourceState::Present);
      }

      m_swapchain->present();

      m_render_engine->new_frame();
      m_compute_engine->new_frame();
      m_copy_engine->new_frame();

      {
        auto render_ctx = new_render_ctx();
        render_ctx->transition_buffer(m_swapchain->buffer(m_swapchain->current_buffer_idx()), rhi::ResourceState::RenderTarget);
      }

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

    rhi::RenderTarget* GpuEngine::render_target()
    {
      return m_swapchain_render_targets[m_swapchain->current_buffer_idx()].get();
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
      return m_copy_engine->new_context<rhi::CopyContext>();
    }

    ScopedPoolObject<rhi::RenderContext> GpuEngine::new_render_ctx()
    {
      return m_render_engine->new_context<rhi::RenderContext>();
    }

    ScopedPoolObject<rhi::ComputeContext> GpuEngine::new_compute_ctx()
    {
      return m_compute_engine->new_context<rhi::ComputeContext>();
    }

  }
}