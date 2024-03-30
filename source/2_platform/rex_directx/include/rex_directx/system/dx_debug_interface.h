#pragma once

#include "rex_directx/utility/dx_util.h"
#include "rex_directx/dxgi/util.h"

namespace rex
{
  namespace rhi
  {
    class DebugInterface
    {
    public:
      DebugInterface();
      ~DebugInterface();

    private:
      wrl::ComPtr<IDXGIDebug1> m_debug_interface;
    };
  }
}