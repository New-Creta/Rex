#pragma once

#include "rex_directx/dxgi/comobject.h"
#include "rex_directx/wrl/wrl_types.h"
#include "rex_renderer_core/gpu_description.h"
#include "rex_std/bonus/types.h"

struct IDXGIAdapter;

namespace rex
{
  namespace dxgi
  {
    class Adapter : public dxgi::ComObject<IDXGIAdapter> // NOLINT(fuchsia-multiple-inheritance)
    {
    public:
      Adapter(wrl::com_ptr<IDXGIAdapter>&& adapter, uint32 version);

      const GpuDescription& description() const;

    private:
      GpuDescription m_description;
    };
  } // namespace dxgi
} // namespace rex