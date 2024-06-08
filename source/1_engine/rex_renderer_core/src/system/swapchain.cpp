#include "rex_renderer_core/system/swapchain.h"

namespace rex
{
  namespace rhi
  {
    Swapchain::Swapchain(s32 width, s32 height, renderer::TextureFormat format)
      : m_width(width)
      , m_height(height)
      , m_format(format)
    {}

    s32 Swapchain::width() const
    {
      return m_width;
    }
    s32 Swapchain::height() const
    {
      return m_height;
    }
    renderer::TextureFormat Swapchain::format() const
    {
      return m_format;
    }
    s32 Swapchain::num_buffers() const
    {
      return m_swapchain_buffers.size();
    }
    RenderTarget* Swapchain::current_buffer()
    {
      return m_swapchain_buffers[current_buffer_idx()].get();
    }

    void Swapchain::clear_buffers()
    {
      m_swapchain_buffers.clear();
    }
    void Swapchain::store_render_target(rsl::unique_ptr<RenderTarget> buffer)
    {
      m_swapchain_buffers.emplace_back(rsl::move(buffer));
    }
    void Swapchain::on_resize(s32 width, s32 height)
    {
      m_width = width;
      m_height = height;
    }

  }
}