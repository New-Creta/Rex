#include "rex_renderer_core/gfx/gpu_engine.h"

#include "rex_renderer_core/resources/render_target.h"
#include "rex_renderer_core/resources/texture_2d.h"

#include "rex_renderer_core/imgui/imgui_device.h"

#include "rex_engine/engine/defines.h"
#include "rex_engine/diagnostics/log.h"

namespace rex
{
  namespace gfx
  {
    DEFINE_LOG_CATEGORY(LogGpuEngine);

    // Make sure to not try and initialize any gpu resources in the constructor.
    // The derived class of the gpu engine is responsible for making sure the gpu is ready.
    GpuEngine::GpuEngine(rsl::unique_ptr<RenderEngine> renderEngine, rsl::unique_ptr<ComputeEngine> computeEngine, rsl::unique_ptr<CopyEngine> copyEngine, const renderer::OutputWindowUserData& userData)
      : m_render_engine(rsl::move(renderEngine))
      , m_compute_engine(rsl::move(computeEngine))
      , m_copy_engine(rsl::move(copyEngine))
      , m_swapchain()
      , m_max_frames_in_flight(userData.max_frames_in_flight)
      , m_primary_display_handle(userData.primary_display_handle)
    {
      init_clear_state();
    }

    void GpuEngine::init()
    {
      REX_WARN_ONCE(LogGpuEngine, "Verify if we can't remove GpuEngine's post init function");
      m_swapchain = rhi::create_swapchain(m_render_engine->command_queue(), m_max_frames_in_flight, m_primary_display_handle);
      init_swapchain_render_targets();
      init_imgui();

      m_copy_engine->init();
    }

    // Prepare a new frame
    void GpuEngine::new_frame()
    {
      auto render_ctx = new_render_ctx();
      render_ctx->transition_buffer(m_swapchain->buffer(m_swapchain->current_buffer_idx()), rhi::ResourceState::RenderTarget);
      render_ctx->clear_render_target(render_target(), m_clear_state_resource.get());
    }
    void GpuEngine::present()
    {
      auto render_ctx = new_render_ctx();
      render_ctx->transition_buffer(m_swapchain->buffer(m_swapchain->current_buffer_idx()), rhi::ResourceState::Present);
      render_ctx->execute_on_gpu();

      m_swapchain->present();

      m_render_engine->new_frame();
      m_compute_engine->new_frame();
      m_copy_engine->new_frame();
    }
    void GpuEngine::end_frame()
    {
      // Nothing to implement
    }

    ScopedPoolObject<rhi::CopyContext> GpuEngine::new_copy_ctx()
    {
      auto base_ctx = m_copy_engine->new_context();
      return base_ctx.convert<rhi::CopyContext>();
    }
    ScopedPoolObject<rhi::RenderContext> GpuEngine::new_render_ctx()
    {
      auto base_ctx = m_render_engine->new_context();
      return base_ctx.convert<rhi::RenderContext>();
    }
    ScopedPoolObject<rhi::ComputeContext> GpuEngine::new_compute_ctx()
    {
      auto base_ctx = m_compute_engine->new_context();
      return base_ctx.convert<rhi::ComputeContext>();
    }

    rhi::RenderTarget* GpuEngine::render_target()
    {
      REX_WARN_ONCE(LogGpuEngine, "Remove render target getter of gpu engine");
      return m_swapchain_render_targets[m_swapchain->current_buffer_idx()].get();
    }

    void GpuEngine::init_clear_state()
    {
      rhi::ClearStateDesc desc{};
      desc.rgba = rsl::colors::LightSteelBlue;
      desc.flags.add_state(renderer::ClearBits::ClearColorBuffer);
      m_clear_state_resource = rsl::make_unique<rhi::ClearStateResource>(desc);
    }
    void GpuEngine::init_imgui()
    {
      ImGuiDevice imgui_device{};
      imgui_device.command_queue = m_render_engine->command_queue();
      imgui_device.max_num_frames_in_flight = m_max_frames_in_flight;
      imgui_device.rtv_format = m_swapchain->format();
      init_imgui_device(imgui_device);
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
  }
}