#pragma once

#include "rex_directx/dxgi/util.h"
#include "rex_directx/utility/dx_util.h"

namespace rex
{
  namespace rhi
  {
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
      wrl::ComPtr<IDXGIDebug1> m_debug_interface;
    };
  } // namespace rhi
} // namespace rex