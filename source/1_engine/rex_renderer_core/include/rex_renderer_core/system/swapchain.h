#pragma once

#include "rex_renderer_core/resources/texture_2d.h"
#include "rex_renderer_core/rhi/texture_format.h"

namespace rex
{
  namespace rhi
  {
    class Swapchain
    {
    public:
      virtual ~Swapchain() = default;
      virtual void present() = 0;

      s32 width() const;
      s32 height() const;
      renderer::TextureFormat format() const;
      s32 num_buffers() const;
      Texture2D* buffer(s32 idx);

    protected:
      void clear_buffers();
      void store_buffer(rsl::unique_ptr<Texture2D> buffer);
      void on_resize(s32 width, s32 height);

    private:
      rsl::vector<rsl::unique_ptr<Texture2D>> m_swapchain_buffers;
      s32 m_width;
      s32 m_height;
      renderer::TextureFormat m_format;
    };
  }
}