#pragma once

#include "rex_directx/dxgi/comobject.h"
#include "rex_engine/engine/types.h"
#include "rex_engine/platform/win/win_com_ptr.h"
#include "rex_renderer_core/system/gpu_description.h"
#include "rex_std/bonus/types.h"

#include "rex_directx/utility/dx_util.h"

struct IDXGIAdapter;

namespace rex
{
  namespace rhi
  {
    class DxDevice;
  }

  namespace dxgi
  {
    class Adapter : public dxgi::DxgiObject<IDXGIAdapter> // NOLINT(fuchsia-multiple-inheritance)
    {
    public:
      Adapter(wrl::ComPtr<IDXGIAdapter>&& adapter, u32 version);

      // Get human readable description of this device
      const GpuDescription& description() const;

      // Create the directx device from this adapter
      rsl::unique_ptr<rhi::DxDevice> create_device() const;

    private:
      // Query the highest supported feature level on this adapter
      D3D_FEATURE_LEVEL query_highest_feature_level();

    private:
      GpuDescription m_description;
      D3D_FEATURE_LEVEL m_highest_feature_level;
    };
  } // namespace dxgi
} // namespace rex