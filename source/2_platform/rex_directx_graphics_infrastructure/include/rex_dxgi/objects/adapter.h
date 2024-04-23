#pragma once

#include "rex_dxgi/wrl/comobject.h"
#include "rex_engine/engine/types.h"
#include "rex_engine/platform/win/win_com_ptr.h"
#include "rex_renderer_core/gpu_description.h"
#include "rex_std/bonus/types.h"

struct IDXGIAdapter4;

namespace rex
{
  namespace dxgi
  {
    class Adapter : public wrl::ComObject<IDXGIAdapter4> // NOLINT(fuchsia-multiple-inheritance)
    {
    public:
      Adapter(wrl::ComPtr<IDXGIAdapter4>&& adapter);

      const renderer::GpuDescription& description() const;

    private:
      renderer::GpuDescription m_description;
    };
  } // namespace dxgi
} // namespace rex