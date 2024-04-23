#pragma once

#include "rex_engine/engine/types.h"

#include "rex_directx/directx_util.h"
#include "rex_directx/resources/render_target.h"

struct IDXGISwapChain4;

namespace rex
{
  namespace renderer
  {
    class Device;
    class Texture;
    class CommandQueue;

    class Swapchain
    {
    public:
      static constexpr u32 num_back_buffers()
      {
        return s_buffer_count;
      }

    public:
      /**
       * Check to see if the swap chain is in full-screen exclusive mode.
       */
      bool is_fullscreen() const;

      /**
       * Set the swap chain to fullscreen exclusive mode (true) or windowed mode (false).
       */
      void set_fullscreen(bool fullscreen);

      /**
       * Toggle fullscreen exclusive mode.
       */
      void toggle_fullscreen();

      void set_v_sync(bool vSync);

      bool v_sync() const;

      void toggle_v_sync();

      /**
       * Check to see if tearing is supported.
       *
       * @see https://docs.microsoft.com/en-us/windows/win32/direct3ddxgi/variable-refresh-rate-displays
       */
      bool is_tearing_supported() const;

      /**
       * Block the current thread until the swapchain has finished presenting.
       * Doing this at the beginning of the update loop can improve input latency.
       */
      void wait_for_swapchain();

      /**
       * Resize the swapchain's back buffers. This should be called whenever the window is resized.
       */
      bool on_resize(s32 clientWidth, s32 clientHeight);

     /**
       * Get the render target of the window. This method should be called every
       * frame since the color attachment point changes depending on the window's
       * current back buffer.
       */
      RenderTarget& render_target();

      /**
       * Get the render target of the window. This method should be called every
       * frame since the color attachment point changes depending on the window's
       * current back buffer.
       */
      const RenderTarget& render_target() const;

      /**
       * Present the swapchain's back buffer to the screen.
       *
       * @param [texture] The texture to copy to the swap chain's backbuffer before
       * presenting. By default, this is an empty texture. In this case, no copy
       * will be performed. Use the SwapChain::GetRenderTarget method to get a render
       * target for the window's color buffer.
       *
       * @returns The current backbuffer index after the present.
       */
      u32 present(const rsl::shared_ptr<Texture>& texture = nullptr);

      /**
       * Get the format that is used to create the backbuffer.
       */
      DXGI_FORMAT render_target_format() const;

      wrl::ComPtr<IDXGISwapChain4> dxgi_swap_chain() const;

    protected:
      // Swap chains can only be created through the Device.
      Swapchain(Device& device, HWND hWnd, s32 clientWidth, s32 clientHeight, DXGI_FORMAT renderTargetFormat = DXGI_FORMAT_R10G10B10A2_UNORM);
      virtual ~Swapchain();

      bool update_render_target_views();

    private:
      // Number of swapchain back buffers.
      static constexpr u32 s_buffer_count = 3;

    private:
      // The device that was used to create the SwapChain.
      Device& m_device;

      // The command queue that is used to create the swapchain.
      // The command queue will be signaled right after the Present
      // to ensure that the swapchain's back buffers are not in-flight before
      // the next frame is allowed to be rendered.
      CommandQueue& m_command_queue;

      wrl::ComPtr<IDXGISwapChain4> m_dxgi_swap_chain;
      rsl::shared_ptr<Texture> m_back_buffer_textures[s_buffer_count];
      mutable RenderTarget m_render_target;

      // The current backbuffer index of the swap chain.
      u32 m_current_back_buffer_index;
      u64 m_fence_values[s_buffer_count]; // The fence values to wait for before leaving the Present method.

      // A handle to a waitable object. Used to wait for the swapchain before presenting.
      HANDLE m_h_frame_latency_waitable_object;

      // The window handle that is associates with this swap chain.
      HWND m_hwnd;

      // The current width/height of the swap chain.
      s32 m_width;
      s32 m_height;

      // The format of the back buffer.
      DXGI_FORMAT m_render_target_format;

      // Should presentation be synchronized with the vertical refresh rate of the screen?
      // Set to true to eliminate screen tearing.
      bool m_v_sync;

      // Whether or not tearing is supported.
      bool m_tearing_supported;

      // Whether the application is in full-screen exclusive mode or windowed mode.
      bool m_fullscreen;
    };
  } // namespace renderer
} // namespace rex