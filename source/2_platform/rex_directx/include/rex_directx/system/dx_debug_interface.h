#pragma once

#include "rex_directx/dxgi/includes.h"
#include "rex_directx/utility/dx_util.h"

namespace rex
{
  namespace gfx
  {
    // DirectX debug interface
    // It enables all debug levels when constructed
    // Reports remaining live objects on destruction
    class DebugInterface
    {
    public:
      DebugInterface();
      DebugInterface(const DebugInterface&) = delete;
      DebugInterface(DebugInterface&&) = delete;
      ~DebugInterface();

      DebugInterface& operator=(const DebugInterface&) = delete;
      DebugInterface& operator=(DebugInterface&&) = delete;

    private:
      void init_dxgi_debug_interface();
      void init_debug_controller();
      void init_debug_info_queue();

    private:
      wrl::ComPtr<IDXGIDebug1> m_debug_interface;
      wrl::ComPtr<ID3D12Debug> m_debug_controller;
      wrl::ComPtr<IDXGIInfoQueue> m_debug_info_queue;
    };
  } // namespace gfx
} // namespace rex