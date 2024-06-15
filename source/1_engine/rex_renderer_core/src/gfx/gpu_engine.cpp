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
    GpuEngine::GpuEngine(const renderer::OutputWindowUserData& userData)
      : m_render_engine()
      , m_compute_engine()
      , m_copy_engine()
      , m_swapchain()
      , m_max_frames_in_flight(userData.max_frames_in_flight)
      , m_primary_display_handle(userData.primary_display_handle)
      , m_frame_idx(0)
    {
    }

    // Because some objects have a dependency on the gpu engine itself
    // We need to wait for the gpu engine to be constructed, only then 
    // can we initialize the rest of the objects
    void GpuEngine::init()
    {
      init_resource_heap();
      init_desc_heaps();
      init_sub_engines();
      init_clear_state();
      init_swapchain();
      init_imgui();
    }

    // Prepare a new frame by incrementing the frame index and clearing the backbuffer
    void GpuEngine::new_frame()
    {
      ++m_frame_idx;

      m_render_engine->new_frame();
      m_compute_engine->new_frame();
      m_copy_engine->new_frame();

      auto render_ctx = new_render_ctx();
      render_ctx->transition_buffer(current_backbuffer_rt(), rhi::ResourceState::RenderTarget);
      render_ctx->clear_render_target(current_backbuffer_rt(), m_clear_state_resource.get());
    }
    // Present the new frame to the main window
    void GpuEngine::present()
    {
      auto render_ctx = new_render_ctx();
      render_ctx->transition_buffer(current_backbuffer_rt(), rhi::ResourceState::Present);
      render_ctx->execute_on_gpu();

      m_swapchain->present();
    }
    // Finish off the last frame
    void GpuEngine::end_frame()
    {
      m_render_engine->end_frame();
      m_compute_engine->end_frame();
      m_copy_engine->end_frame();
    }

    // Create a new context which is used for copying resources from or to the gpu
    ScopedPoolObject<rhi::CopyContext> GpuEngine::new_copy_ctx()
    {
      auto base_ctx = m_copy_engine->new_context(desc_heap(rhi::DescriptorHeapType::ShaderResourceView));
      return base_ctx.convert<rhi::CopyContext>();
    }
    // Create a new context which is used for rendering to render targets
    ScopedPoolObject<rhi::RenderContext> GpuEngine::new_render_ctx()
    {
      auto base_ctx = m_render_engine->new_context(desc_heap(rhi::DescriptorHeapType::ShaderResourceView));
      return base_ctx.convert<rhi::RenderContext>();
    }
    // Create a new context which is used for computing data on the gpu
    ScopedPoolObject<rhi::ComputeContext> GpuEngine::new_compute_ctx()
    {
      auto base_ctx = m_compute_engine->new_context(desc_heap(rhi::DescriptorHeapType::ShaderResourceView));
      return base_ctx.convert<rhi::ComputeContext>();
    }

    // Return the render target pointing to the current backbuffer of the swapchain
    rhi::RenderTarget* GpuEngine::current_backbuffer_rt()
    {
      return m_swapchain->current_buffer();
    }

    // Returns a specific descriptor heap based on type
    rhi::DescriptorHeap* GpuEngine::desc_heap(rhi::DescriptorHeapType descHeapType)
    {
      return m_descriptor_heap_pool.at(descHeapType).get();
    }

    // Initialize the clear state which is used to clear the backbuffer with
    void GpuEngine::init_clear_state()
    {
      rhi::ClearStateDesc desc{};
      desc.rgba = rsl::colors::LightSteelBlue;
      desc.flags.add_state(renderer::ClearBits::ClearColorBuffer);
      m_clear_state_resource = rsl::make_unique<rhi::ClearState>(desc);
    }
    // Initialize the swapchain which is used for presenting to the main window
    void GpuEngine::init_swapchain()
    {
      m_swapchain = rhi::create_swapchain(m_render_engine->command_queue(), m_max_frames_in_flight, m_primary_display_handle);
    }
    // Initialize the sub engine, bringing them up and ready, to be used in the graphics pipeline
    void GpuEngine::init_sub_engines()
    {
      m_render_engine = init_render_engine(&m_resource_state_tracker);
      m_copy_engine = init_copy_engine(&m_resource_state_tracker);
      m_compute_engine = init_compute_engine(&m_resource_state_tracker);

      m_render_engine->init();
      m_copy_engine->init();
      m_compute_engine->init();
    }
    // Initialize imgui so it can create its own windows if necessary
    void GpuEngine::init_imgui()
    {
      ImGuiDevice imgui_device{};
      imgui_device.command_queue = m_render_engine->command_queue();
      imgui_device.max_num_frames_in_flight = m_max_frames_in_flight;
      imgui_device.rtv_format = m_swapchain->format();
      init_imgui_device(imgui_device);
    }
    // Initialize the descriptor heaps which keep track of all descriptors to various resources
    void GpuEngine::init_desc_heaps()
    {
      init_desc_heap(rhi::DescriptorHeapType::RenderTargetView);
      init_desc_heap(rhi::DescriptorHeapType::DepthStencilView);
      init_desc_heap(rhi::DescriptorHeapType::ConstantBufferView);
      init_desc_heap(rhi::DescriptorHeapType::Sampler);
    }
    void GpuEngine::init_desc_heap(rhi::DescriptorHeapType descHeapType)
    {
      m_descriptor_heap_pool.emplace(descHeapType, allocate_desc_heap(descHeapType));

    }

  }
}