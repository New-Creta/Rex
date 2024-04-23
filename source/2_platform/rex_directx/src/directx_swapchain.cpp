#include "rex_directx/directx_swapchain.h"
#include "rex_directx/directx_call.h"
#include "rex_directx/directx_device.h"
#include "rex_directx/command_list.h"
#include "rex_directx/command_queue.h"
#include "rex_directx/resources/resource_state_tracker.h"
#include "rex_directx/resources/texture.h"
#include "rex_directx/log.h"

namespace rex
{
  namespace renderer
  {
    Swapchain::Swapchain(Device& device, HWND hwnd, s32 clientWidth, s32 clientHeight, DXGI_FORMAT renderTargetFormat)
        : m_device(device)
        , m_command_queue(device.command_queue(D3D12_COMMAND_LIST_TYPE_DIRECT))
        , m_hwnd(hwnd)
        , m_fence_values {0}
        , m_width(clientWidth)
        , m_height(clientHeight)
        , m_render_target_format(renderTargetFormat)
        , m_v_sync(true)
        , m_tearing_supported(false)
        , m_fullscreen(false)
    {
      assert(hwnd); // Must be a valid window handle!

      // Query the direct command queue from the device.
      // This is required to create the Swapchain.
      auto d3d_command_queue = m_command_queue.d3d_command_queue();

      // Query the factory from the dxgi_adapter that was used to create the device.
      auto dxgi_adapter = m_device.dxgi_adapter();

      // Get the factory that was used to create the dxgi_adapter.
      wrl::ComPtr<IDXGIFactory> dxgi_factory;
      wrl::ComPtr<IDXGIFactory5> dxgi_factory5;

      HRESULT hr = S_OK;

      hr = dxgi_adapter->GetParent(IID_PPV_ARGS(&dxgi_factory));
      assert(SUCCEEDED(hr));

      // Now get the DXGIFactory5 so I can use the IDXGIFactory5::CheckFeatureSupport method.
      hr = dxgi_factory.As(&dxgi_factory5);
      assert(SUCCEEDED(hr));

      // Check for tearing support.
      BOOL allow_tearing = FALSE;
      hr                 = dxgi_factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allow_tearing, sizeof(BOOL));
      if(SUCCEEDED(hr))
      {
        m_tearing_supported = (allow_tearing == TRUE);
      }

      // Query the windows client width and height.
      RECT window_rect;
      ::GetClientRect(hwnd, &window_rect);

      //m_width  = window_rect.right - window_rect.left;
      //m_height = window_rect.bottom - window_rect.top;

      DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {};
      swap_chain_desc.Width                 = m_width;
      swap_chain_desc.Height                = m_height;
      swap_chain_desc.Format                = m_render_target_format;
      swap_chain_desc.Stereo                = FALSE;
      swap_chain_desc.SampleDesc            = {1, 0};
      swap_chain_desc.BufferUsage           = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      swap_chain_desc.BufferCount           = s_buffer_count;
      swap_chain_desc.Scaling               = DXGI_SCALING_STRETCH;
      swap_chain_desc.SwapEffect            = DXGI_SWAP_EFFECT_FLIP_DISCARD;
      swap_chain_desc.AlphaMode             = DXGI_ALPHA_MODE_UNSPECIFIED;
      // It is recommended to always allow tearing if tearing support is available.
      swap_chain_desc.Flags = m_tearing_supported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
      swap_chain_desc.Flags |= DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

      // Now create the swap chain.
      wrl::ComPtr<IDXGISwapChain1> dxgi_swap_chain1;
      hr = dxgi_factory5->CreateSwapChainForHwnd(d3d_command_queue.Get(), m_hwnd, &swap_chain_desc, nullptr, nullptr, &dxgi_swap_chain1);
      assert(SUCCEEDED(hr));

      // Cast to swapchain4
      hr = dxgi_swap_chain1.As(&m_dxgi_swap_chain);
      assert(SUCCEEDED(hr));

      // Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
      // will be handled manually.
      hr = dxgi_factory5->MakeWindowAssociation(m_hwnd, DXGI_MWA_NO_ALT_ENTER);
      assert(SUCCEEDED(hr));

      // Initialize the current back buffer index.
      m_current_back_buffer_index = m_dxgi_swap_chain->GetCurrentBackBufferIndex();

      // Set maximum frame latency to reduce input latency.
      m_dxgi_swap_chain->SetMaximumFrameLatency(s_buffer_count - 1);
      // Get the SwapChain's waitable object.
      m_h_frame_latency_waitable_object = m_dxgi_swap_chain->GetFrameLatencyWaitableObject();

      update_render_target_views();
    }

    Swapchain::~Swapchain() = default;

    bool Swapchain::is_fullscreen() const
    {
      return m_fullscreen;
    }

    void Swapchain::set_fullscreen(bool fullscreen)
    {
      if(m_fullscreen != fullscreen)
      {
        m_fullscreen = fullscreen;
      }
    }

    void Swapchain::toggle_fullscreen()
    {
      set_fullscreen(!m_fullscreen);
    }

    void Swapchain::set_v_sync(bool vSync)
    {
      m_v_sync = vSync;
    }

    bool Swapchain::v_sync() const
    {
      return m_v_sync;
    }

    void Swapchain::toggle_v_sync()
    {
      set_v_sync(!m_v_sync);
    }

    bool Swapchain::is_tearing_supported() const
    {
      return m_tearing_supported;
    }

    void Swapchain::wait_for_swapchain()
    {
      ::WaitForSingleObjectEx(m_h_frame_latency_waitable_object, 1000, TRUE); // Wait for 1 second (should never have to wait that long...)
    }

    bool Swapchain::on_resize(s32 clientWidth, s32 clientHeight)
    {
      if(m_width != clientWidth || m_height != clientHeight)
      {
        m_width  = (rsl::max)(1, clientWidth);
        m_height = (rsl::max)(1, clientHeight);

        m_device.flush();

        // Release all references to back buffer textures.
        m_render_target.reset();

        for(u32 i = 0; i < s_buffer_count; ++i)
        {
          m_back_buffer_textures[i].reset();
        }

        DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
        if(DX_FAILED(m_dxgi_swap_chain->GetDesc(&swap_chain_desc)))
        {
          REX_ERROR(LogDirectX, "Failed to retrieve Swapchain description");
          return false;
        }

        if(DX_FAILED(m_dxgi_swap_chain->ResizeBuffers(s_buffer_count, m_width, m_height, swap_chain_desc.BufferDesc.Format, swap_chain_desc.Flags)))
        {
          REX_ERROR(LogDirectX, "Failed to resize buffers");
          return false;
        }

        m_current_back_buffer_index = m_dxgi_swap_chain->GetCurrentBackBufferIndex();

        update_render_target_views();
      }

      return true;
    }

    RenderTarget& Swapchain::render_target()
    {
      m_render_target.attach_texture(AttachmentPoint::Color0, m_back_buffer_textures[m_current_back_buffer_index]);
      return m_render_target;
    }

    const RenderTarget& Swapchain::render_target() const
    {
      m_render_target.attach_texture(AttachmentPoint::Color0, m_back_buffer_textures[m_current_back_buffer_index]);
      return m_render_target;
    }

    u32 Swapchain::present(const rsl::shared_ptr<Texture>& texture)
    {
      auto command_list = m_command_queue.command_list();
      auto back_buffer  = m_back_buffer_textures[m_current_back_buffer_index];

      if(texture)
      {
        if(texture->d3d_resource_desc().SampleDesc.Count > 1)
        {
          command_list->resolve_subresource(back_buffer, texture);
        }
        else
        {
          command_list->copy_resource(back_buffer, texture);
        }
      }

      command_list->transition_barrier(back_buffer, D3D12_RESOURCE_STATE_PRESENT);

      m_command_queue.execute_command_list(command_list);

      u32 sync_interval = m_v_sync ? 1 : 0;
      u32 present_flags = m_tearing_supported && !m_fullscreen && !m_v_sync ? DXGI_PRESENT_ALLOW_TEARING : 0;
      if(DX_FAILED(m_dxgi_swap_chain->Present(sync_interval, present_flags)))
      {
        REX_ERROR(LogDirectX, "Unable to present!");
        return static_cast<u32>(-1);
      }

      m_fence_values[m_current_back_buffer_index] = m_command_queue.signal();

      m_current_back_buffer_index = m_dxgi_swap_chain->GetCurrentBackBufferIndex();

      auto fence_value = m_fence_values[m_current_back_buffer_index];
      m_command_queue.wait_for_fence_value(fence_value);

      m_device.release_stale_descriptors();

      return m_current_back_buffer_index;
    }

    DXGI_FORMAT Swapchain::render_target_format() const
    {
      return m_render_target_format;
    }

    wrl::ComPtr<IDXGISwapChain4> Swapchain::dxgi_swap_chain() const
    {
      return m_dxgi_swap_chain;
    }

    bool Swapchain::update_render_target_views()
    {
      for(u32 i = 0; i < s_buffer_count; ++i)
      {
        wrl::ComPtr<ID3D12Resource> back_buffer;
        if(DX_FAILED(m_dxgi_swap_chain->GetBuffer(i, IID_PPV_ARGS(&back_buffer))))
        {
          REX_ERROR(LogDirectX, "Unable to retrieve Swapchain buffer");
          return false;
        }

        ResourceStateTracker::add_global_resource_state(back_buffer.Get(), D3D12_RESOURCE_STATE_COMMON);

        m_back_buffer_textures[i] = m_device.create_texture(back_buffer);

        // Set the names for the backbuffer textures.
        // Useful for debugging.
        m_back_buffer_textures[i]->set_resource_name(L"Backbuffer[" + rsl::to_wstring(i) + L"]");
      }

      return true;
    }
  } // namespace renderer
} // namespace rex