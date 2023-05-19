#pragma once

#include "rex_directx/dxgi/comobject.h"
#include "rex_directx/wrl/wrl_types.h"

struct IDXGIFactory;

#include "rex_std/bonus/types.h"

namespace rex
{
  namespace dxgi
  {
    class Factory : public ComObject<IDXGIFactory>
    {
    public:
      static Factory create();

      Factory();
      Factory(wrl::com_ptr<IDXGIFactory>&& object, uint32 version);
    };
  } // namespace dxgi
} // namespace rex