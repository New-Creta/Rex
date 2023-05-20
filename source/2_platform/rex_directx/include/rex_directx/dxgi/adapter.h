#pragma once

#include "rex_directx/dxgi/comobject.h"
#include "rex_directx/wrl/wrl_types.h"
#include "rex_renderer_core/gpu.h"

#include "rex_std/bonus/types.h"

struct IDXGIAdapter;

namespace rex
{
  namespace dxgi
  {
    class Adapter : public Gpu, public dxgi::ComObject<IDXGIAdapter> // NOLINT(fuchsia-multiple-inheritance)
    {
    public:
      Adapter(wrl::com_ptr<IDXGIAdapter>&& adapter, uint32 version);
    };
  } // namespace dxgi
} // namespace rex