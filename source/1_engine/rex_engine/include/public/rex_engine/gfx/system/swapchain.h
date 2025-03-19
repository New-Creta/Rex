#pragma once

#include "rex_engine/gfx/resources/render_target.h"
#include "rex_engine/gfx/core/texture_format.h"

namespace rex
{
  namespace gfx
  {
    class Swapchain
    {
    public:
      Swapchain(s32 width, s32 height, TextureFormat format);
      virtual ~Swapchain() = default;

      // Swap the back with the front buffer
      virtual void present() = 0;

      // Return the width of the buffers of the swapchain
      s32 width() const;
      // Return the height of the buffers of the swapchain
      s32 height() const;
      // Return the format of the buffers of the swapchain
      TextureFormat format() const;
      // Return the number of buffers of the swapchain
      s32 num_buffers() const;
      // Return the current backbuffer as a render target
      RenderTarget* current_buffer();

      // Return the index of the current bufffer
      virtual s32 current_buffer_idx() const = 0;

    protected:
      // Empty the list of buffers
      void clear_buffers();
      // Store a new render target pointing to one of the back buffers
      void store_render_target(rsl::unique_ptr<RenderTarget> buffer);
      // called on resize to change the cached width and height
      void on_resize(s32 width, s32 height);

    private:
      rsl::vector<rsl::unique_ptr<RenderTarget>> m_swapchain_buffers;
      s32 m_width;
      s32 m_height;
      TextureFormat m_format;
    };
  }
}