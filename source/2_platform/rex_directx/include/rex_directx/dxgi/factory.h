#pragma once

#include "rex_directx/dxgi/comobject.h"  // for ComObject
#include "rex_directx/wrl/wrl_types.h"   // for com_ptr
#include "rex_engine/types.h"
#include "rex_std/memory.h"
#include "rex_std/bonus/types.h"         // for uint32

struct IDXGIFactory;

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