#pragma once

#include "rex_engine/types.h"
#include "rex_directx/dxgi/comobject.h"
#include "rex_directx/wrl/wrl_types.h"

struct IDXGIFactory;

namespace rex
{
  namespace dxgi
  {
    class Factory : public dxgi::ComObject<IDXGIFactory>
    {
    public:
      static Factory create();

      Factory();
      Factory(wrl::com_ptr<IDXGIFactory>&& object, u32 version);
    };
  } // namespace dxgi
} // namespace rex