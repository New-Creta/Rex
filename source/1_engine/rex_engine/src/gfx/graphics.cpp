#include "rex_engine/gfx/graphics.h"
#include "rex_engine/gfx/system/gpu_engine.h"

#include "rex_engine/gfx/system/rhi.h"
#include "rex_engine/gfx/core/gpu_description.h"
#include "rex_engine/gfx/system/root_signature_cache.h"

#include "rex_std/bonus/utility.h"
#include "rex_std/memory.h"

namespace rex
{
  namespace gfx
  {
    DEFINE_LOG_CATEGORY(LogGraphics);

    // We can access it in the high level graphics api, but the gpu engine is owned by the rhi
    GpuEngine* g_gpu_engine;
    rsl::vector<rsl::unique_ptr<Renderer>> g_renderers;

    rsl::array<RasterStateDesc, rsl::enum_refl::enum_count<CommonRasterState>()> g_common_raster_states;
    rsl::array<rsl::unique_ptr<Sampler2D>, rsl::enum_refl::enum_count<CommonSampler>()> g_common_samplers;

    namespace internal
    {
      void add_renderer(rsl::unique_ptr<Renderer> renderer)
      {
        g_renderers.emplace_back(rsl::move(renderer));
      }
      void init_common_resources()
      {
        // common raster states
        RasterStateDesc raster_state{};
        raster_state.fill_mode = rex::gfx::FillMode::Solid;
        raster_state.cull_mode = rex::gfx::CullMode::Back;
        raster_state.front_ccw = false;
        raster_state.depth_clip_enable = true;
        g_common_raster_states[rsl::enum_refl::enum_index(CommonRasterState::DefaultDepth).value()] = raster_state;

        raster_state.depth_clip_enable = false;
        g_common_raster_states[rsl::enum_refl::enum_index(CommonRasterState::DefaultNoDepth).value()] = raster_state;

        // common samplers
        SamplerDesc sampler_desc{};
        sampler_desc.filtering = rex::gfx::SamplerFiltering::MinMagMipPoint;
        sampler_desc.address_mode_u = rex::gfx::TextureAddressMode::Wrap;
        sampler_desc.address_mode_v = rex::gfx::TextureAddressMode::Wrap;
        sampler_desc.address_mode_w = rex::gfx::TextureAddressMode::Wrap;
        sampler_desc.mip_lod_bias = 0.0f;
        sampler_desc.max_anisotropy = 0;
        sampler_desc.comparison_func = rex::gfx::ComparisonFunc::Always;
        sampler_desc.border_color = rex::gfx::BorderColor::TransparentBlack;
        sampler_desc.min_lod = 0.0f;
        sampler_desc.max_lod = 0.0f;
        sampler_desc.shader_register = 0;
        sampler_desc.register_space = 0;
        sampler_desc.shader_visibility = rex::gfx::ShaderVisibility::Pixel;
        g_common_samplers[rsl::enum_refl::enum_index(CommonSampler::Default2D).value()] = rhi::create_sampler2d(sampler_desc);
      }
    }

    void log_info()
    {
      auto& gfx_info = rhi::info();
      REX_UNUSED_PARAM(gfx_info);

      REX_INFO(LogGraphics, "Renderer Info - Adaptor: {}", gfx_info.adaptor);
      REX_INFO(LogGraphics, "Renderer Info - Vendor: {}", gfx_info.vendor);
      REX_INFO(LogGraphics, "Renderer Info - API: {}", gfx_info.api);
      REX_INFO(LogGraphics, "Renderer Info - API Version: {}", gfx_info.api_version);
      REX_INFO(LogGraphics, "Renderer Info - Shader Version: {}", gfx_info.shader_version);
      REX_INFO(LogGraphics, "Renderer Info - Driver Version: {}", gfx_info.driver_version);
    }

    Error init(const OutputWindowUserData& userData)
    {
      // Initialize the gpu engine, it's responsible for the entire graphics pipeline.
      // The gpu engine gets created by the rhi as some of the objects wrapped by the gpu engine
      // are also needed by the rhi to create other objects.
      g_gpu_engine = rhi::init(userData);
      rsl::scopeguard shutdown_on_failure = []() { shutdown(); };

      // If we even fail to allocate the memory for the gpu engine, we exit immediately.
      // This is possible if we OOM or if any pre allocation checks fail in the rhi
      // and it has returns before even getting to the gpu engine initialization step
      if (!g_gpu_engine)
      {
        return Error("Failed to create GPU Engine");
      }

      // Init the common resources so that we don't have to repeat creating the same resources
      internal::init_common_resources();

      // Make sure we release the scopeguard which would shutdown our graphics systems otherwise
      shutdown_on_failure.release();
      return Error::no_error();
    }
    void shutdown()
    {
      g_renderers.clear();
      root_signature_cache::clear();

      rhi::shutdown();
    }

    void render()
    {
      // The render loop is very simple
      // - Prepare a new frame
      // - Loop over all the renderers
      // - Finalize the frame
      // - Present

      // Prepare all resources that are needed for a new frame
      g_gpu_engine->new_frame();
      
      // Loop over all renderers and call into them
      // Performing all required gpu operations needed on resources those renderers need
      // The resources are owned by the renderers, they're responsible for creating and destroying them
      // However in the backend, the gpu manager controls when these resources actually get deallocated
      for (auto& renderer : g_renderers)
      {
        renderer->render();
      }

      // Finalize the frame and bringing it ready for presentation
      g_gpu_engine->end_frame();

      // Present the last rendered frame to the screen
      g_gpu_engine->present();
    }

    // Create a new copy context that automatically gets added back to the pool when it goes out of scope
    ScopedPoolObject<CopyContext> new_copy_ctx(PipelineState* pso, rsl::string_view eventName)
    {
      return g_gpu_engine->new_copy_ctx(pso, eventName);
    }
    // Create a new render context that automatically gets added back to the pool when it goes out of scope
    ScopedPoolObject<RenderContext> new_render_ctx(PipelineState* pso, rsl::string_view eventName)
    {
      return g_gpu_engine->new_render_ctx(pso, eventName);
    }
    // Create a new compute context that automatically gets added back to the pool when it goes out of scope
    ScopedPoolObject<ComputeContext> new_compute_ctx(PipelineState* pso, rsl::string_view eventName)
    {
      return g_gpu_engine->new_compute_ctx(pso, eventName);
    }

    RenderTarget* swapchain_rt()
    {
      return g_gpu_engine->current_backbuffer_rt();
    }
    s32 swapchain_width()
    {
      return g_gpu_engine->current_backbuffer_rt()->width();
    }
    s32 swapchain_height()
    {
      return g_gpu_engine->current_backbuffer_rt()->height();
    }

    RasterStateDesc common_raster_state(CommonRasterState type)
    {
      return g_common_raster_states.at(rsl::enum_refl::enum_index(type).value());
    }
    Sampler2D* common_sampler(CommonSampler type)
    {
      return g_common_samplers.at(rsl::enum_refl::enum_index(type).value()).get();
    }
  }
}