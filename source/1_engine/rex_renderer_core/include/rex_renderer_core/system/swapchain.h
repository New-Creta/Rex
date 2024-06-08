#pragma once

#include "rex_renderer_core/resources/render_target.h"
#include "rex_renderer_core/rhi/texture_format.h"

namespace rex
{
  namespace rhi
  {
    class Swapchain
    {
    public:
      Swapchain(s32 width, s32 height, renderer::TextureFormat format);
      virtual ~Swapchain() = default;
      virtual void present() = 0;

      s32 width() const;
      s32 height() const;
      renderer::TextureFormat format() const;
      s32 num_buffers() const;
      RenderTarget* buffer(s32 idx);
      RenderTarget* current_buffer();

      virtual s32 current_buffer_idx() const = 0;

    protected:
      void clear_buffers();
      void store_render_target(rsl::unique_ptr<RenderTarget> buffer);
      void on_resize(s32 width, s32 height);

    private:
      rsl::vector<rsl::unique_ptr<RenderTarget>> m_swapchain_buffers;
      s32 m_width;
      s32 m_height;
      renderer::TextureFormat m_format;
    };
  }
}