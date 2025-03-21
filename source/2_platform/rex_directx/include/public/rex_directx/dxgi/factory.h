#pragma once

#include "rex_directx/dxgi/dxgi_object.h"
#include "rex_engine/engine/types.h"
#include "rex_engine/platform/win/win_com_ptr.h"

struct IDXGIFactory;

namespace rex
{
  namespace gfx
  {
    namespace dxgi
    {
      class Factory : public DxgiObject<IDXGIFactory>
      {
      public:
        static rsl::unique_ptr<Factory> create(s32 flags);

        Factory();
        Factory(wrl::ComPtr<IDXGIFactory>&& object, u32 version);
      };
    }
  } // namespace dxgi
} // namespace rex