#include "rex_directx/dxgi/dx_adapter.h"
#include "rex_directx/dxgi/includes.h"
#include "rex_directx/diagnostics/dx_call.h"
#include "rex_directx/system/dx_device.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/engine/types.h"
#include "rex_std/bonus/memory/memory_size.h"
#include "rex_std/bonus/string.h"
#include "rex_std/string.h"

#include <cstdlib>
#include <wrl/client.h>

namespace
{
  const uint32 g_adapter_description_size = rsl::small_stack_string::max_size();
  const rsl::array g_expected_feature_levels = { D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_1 };

  //-------------------------------------------------------------------------
  rsl::string_view feature_level_name(D3D_FEATURE_LEVEL level)
  {
    switch (level)
    {
    case D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_0: return rsl::string_view("D3D_FEATURE_LEVEL_12_0");
    case D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_1: return rsl::string_view("D3D_FEATURE_LEVEL_12_1");
    case D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_1_0_CORE: return rsl::string_view("D3D_FEATURE_LEVEL_1_0_CORE");
    default: return rsl::string_view("Unknown feature level");
    }
  }

  //-------------------------------------------------------------------------
  bool is_correct_feature_level(D3D_FEATURE_LEVEL level)
  {
    return rsl::cend(g_expected_feature_levels) != rsl::find(rsl::cbegin(g_expected_feature_levels), rsl::cend(g_expected_feature_levels), level);
  }

  //-------------------------------------------------------------------------
  /**
   * The VendorId is a unique identifier assigned by the PCI-SIG (Peripheral Component Interconnect Special Interest Group)
   * to identify the manufacturer of a PCI device, including graphics adapters.
   * The VendorId is a 16-bit unsigned integer that is typically included in
   * the PCI Configuration space of the device.
   */
  enum class Vendor
  {
    UNKNOWN = 0,
    AMD     = 0x1002,
    NVIDIA  = 0x10DE,
    INTEL   = 0x163C
  };

  //-------------------------------------------------------------------------
  rsl::small_stack_string vendor_to_string(s32 vendor)
  {
    // Enum reflection is not possible here as the integer values are
    // outside the valid range of values [0, 127] for this enumeration type
    switch(static_cast<Vendor>(vendor))
    {
      case Vendor::AMD: return rsl::small_stack_string("AMD");
      case Vendor::NVIDIA: return rsl::small_stack_string("NVIDIA");
      case Vendor::INTEL: return rsl::small_stack_string("INTEL");
      default: return rsl::small_stack_string("Unknown Vendor");
    }
  }

  //-------------------------------------------------------------------------
  rsl::small_stack_string to_multibyte(const tchar* wideCharacterBuffer, count_t size)
  {
    rsl::small_stack_string buffer;

    // Convert wide character string to multi byte character string.
    // size_t converted_chars => The amount of converted characters.
    // 0 terminate the string afterwards.
    size_t converted_chars = 0;
    auto result            = wcstombs_s(&converted_chars, buffer.data(), size, wideCharacterBuffer, size);
    if(result != 0)
    {
      //REX_ERROR(LogDirectX, "Failed to convert wide character string to multi byte character string.");
      return rsl::small_stack_string("Invalid String");
    }

    buffer.reset_null_termination_offset();

    return rsl::small_stack_string(buffer.data(), static_cast<count_t>(converted_chars)); // NOLINT(readability-redundant-string-cstr)
  }

  //-------------------------------------------------------------------------
  template <typename DXGIAdapterDesc>
  rex::GpuDescription convert_description(const DXGIAdapterDesc& dxgiDesc)
  {
    rex::GpuDescription desc;

    desc.name        = to_multibyte(dxgiDesc.Description, g_adapter_description_size);
    desc.vendor_name = vendor_to_string(dxgiDesc.VendorId);

    desc.vendor_id = dxgiDesc.VendorId;
    desc.device_id = dxgiDesc.DeviceId;

    desc.dedicated_video_memory  = rsl::memory_size(dxgiDesc.DedicatedVideoMemory);
    desc.dedicated_system_memory = rsl::memory_size(dxgiDesc.DedicatedSystemMemory);
    desc.shared_system_memory    = rsl::memory_size(dxgiDesc.SharedSystemMemory);

    return desc;
  }

  //-------------------------------------------------------------------------
  rex::GpuDescription get_description(const rex::wrl::ComPtr<IDXGIAdapter>& adapter)
  {
    rex::GpuDescription desc;

    // MSDN says that you shouldn't use dxgi 1.0 and dxgi 1.1
    // in the same application.
    // so if the version is 1 or higher, we use dxgi 1.1
    rex::wrl::ComPtr<IDXGIAdapter1> adapter_1;
    adapter.As<IDXGIAdapter1>(&adapter_1);
    if(adapter_1)
    {
      DXGI_ADAPTER_DESC1 dxgi_desc;
      adapter_1->GetDesc1(&dxgi_desc);
      desc = convert_description(dxgi_desc);
    }
    else
    {
      DXGI_ADAPTER_DESC dxgi_desc;
      adapter->GetDesc(&dxgi_desc);
      desc = convert_description(dxgi_desc);
    }

    return desc;
  }
} // namespace

namespace rex
{
  namespace dxgi
  {
    DEFINE_LOG_CATEGORY(LogDxAdapter);

    //-------------------------------------------------------------------------
    Adapter::Adapter(wrl::ComPtr<IDXGIAdapter>&& adapter, u32 version)
        : DxgiObject(rsl::move(adapter), version)
        , m_description(::get_description(com_ptr()))
    {
      m_highest_feature_level = query_highest_feature_level();
    }

    //-------------------------------------------------------------------------
    const GpuDescription& Adapter::description() const
    {
      return m_description;
    }

    rsl::unique_ptr<rhi::DxDevice> Adapter::create_device() const
    {
      wrl::ComPtr<ID3D12Device1> d3d_device;
      if (DX_FAILED(D3D12CreateDevice(c_ptr(), static_cast<D3D_FEATURE_LEVEL>(m_highest_feature_level), IID_PPV_ARGS(&d3d_device))))
      {
        REX_ERROR(LogDxAdapter, "Failed to create DX12 Device on {}", description().name);
        return nullptr;
      }

      return rsl::make_unique<rhi::DxDevice>(d3d_device, m_highest_feature_level, this);
    }

    D3D_FEATURE_LEVEL Adapter::query_highest_feature_level()
    {
      // backwards looping as it's checking for a minimum feature level
      for (auto it = g_expected_feature_levels.crbegin(); it != g_expected_feature_levels.crend(); ++it)
      {
        const D3D_FEATURE_LEVEL feature_level = *it;
        if (SUCCEEDED(D3D12CreateDevice(c_ptr(), feature_level, __uuidof(ID3D12Device), nullptr)))
        {
          return feature_level;
        }
      }

      REX_ASSERT("At least D3D_FEATURE_LEVEL_12_0 has to be supported for DirectX 12!");

      // If the compiler doesn't recognise D3D_FEATURE_LEVEL_1_0_CORE
      // Make sure you're using windows SDK 10.0.18362.0 or later
      return D3D_FEATURE_LEVEL_1_0_CORE;
    }

  } // namespace dxgi
} // namespace rex
