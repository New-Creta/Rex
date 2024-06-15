#include "rex_renderer_core/system/swapchain.h"

namespace rex
{
  namespace gfx
  {
    Swapchain::Swapchain(s32 width, s32 height, TextureFormat format)
      : m_width(width)
      , m_height(height)
      , m_format(format)
    {}

    // Return the width of the buffers of the swapchain
    s32 Swapchain::width() const
    {
      return m_width;
    }
    // Return the height of the buffers of the swapchain
    s32 Swapchain::height() const
    {
      return m_height;
    }
    // Return the format of the buffers of the swapchain
    TextureFormat Swapchain::format() const
    {
      return m_format;
    }
    // Return the number of buffers of the swapchain
    s32 Swapchain::num_buffers() const
    {
      return m_swapchain_buffers.size();
    }
    // Return the current backbuffer as a render target
    RenderTarget* Swapchain::current_buffer()
    {
      return m_swapchain_buffers[current_buffer_idx()].get();
    }

    // Empty the list of buffers
    void Swapchain::clear_buffers()
    {
      m_swapchain_buffers.clear();
    }
    // Store a new render target pointing to one of the back buffers
    void Swapchain::store_render_target(rsl::unique_ptr<RenderTarget> buffer)
    {
      m_swapchain_buffers.emplace_back(rsl::move(buffer));
    }
    // called on resize to change the cached width and height
    void Swapchain::on_resize(s32 width, s32 height)
    {
      m_width = width;
      m_height = height;
    }

  }
}