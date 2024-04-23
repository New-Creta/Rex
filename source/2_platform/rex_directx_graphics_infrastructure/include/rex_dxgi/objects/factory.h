#pragma once

#include "rex_dxgi/wrl/comobject.h"

#include "rex_engine/engine/types.h"
#include "rex_engine/platform/win/win_com_ptr.h"

#include "rex_std/memory.h"

struct IDXGIFactory;

namespace rex
{
  namespace dxgi
  {
    class Factory : public wrl::ComObject<IDXGIFactory>
    {
    public:
      static rsl::unique_ptr<Factory> create(u32 flags);

    protected:
      Factory();
      Factory(wrl::ComPtr<IDXGIFactory>&& object);
    };
  } // namespace dxgi
} // namespace rex