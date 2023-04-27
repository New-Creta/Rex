#pragma once

#include "rex_directx/dxgi/comobject.h"
#include "rex_directx/wrl/wrl_types.h"
#include "rex_engine/types.h"
#include "rex_renderer_core/gpu.h"

#include <memory>
#include <string>

struct IDXGIAdapter;

namespace rex
{
  namespace dxgi
  {
    class Adapter : public Gpu, public dxgi::ComObject<IDXGIAdapter>
    {
    public:
      Adapter(wrl::com_ptr<IDXGIAdapter>&& adapter, uint32 version);
    };
  } // namespace dxgi
} // namespace rex