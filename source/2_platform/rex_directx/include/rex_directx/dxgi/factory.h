#pragma once

#include "rex_directx/dxgi/comobject.h"
#include "rex_engine/engine/types.h"
#include "rex_engine/platform/win/win_com_ptr.h"

struct IDXGIFactory;

namespace rex
{
  namespace dxgi
  {
    class Factory : public dxgi::ComObject<IDXGIFactory>
    {
    public:
      static Factory create(s32 flags);

      Factory();
      Factory(wrl::ComPtr<IDXGIFactory>&& object, u32 version);
    };
  } // namespace dxgi
} // namespace rex