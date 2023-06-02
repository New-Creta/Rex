#include "rex_directx/dxgi/adapter.h"

#include "rex_directx/dxgi/util.h"
#include "rex_directx/log.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/types.h"
#include "rex_std/bonus/string.h"
#include "rex_std/string.h"
#include "rex_std_extra/memory/memory_size.h"

#include <cstdlib>
#include <wrl/client.h>

namespace
{
  const uint32 g_adapter_description_size = rsl::small_stack_string::max_size();

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
  rsl::string vendor_to_string(s32 vendor)
  {
    // Enum reflection is not possible here as the integer values are
    // outside the valid range of values [0, 127] for this enumeration type
    switch(static_cast<Vendor>(vendor))
    {
      case Vendor::AMD: return rsl::string("AMD");
      case Vendor::NVIDIA: return rsl::string("NVIDIA");
      case Vendor::INTEL: return rsl::string("INTEL");
      default: return rsl::string("Unknown Vendor");
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
      REX_ERROR(LogDirectX, "Failed to convert wide character string to multi byte character string.");
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
  rex::GpuDescription get_description(const rex::wrl::com_ptr<IDXGIAdapter>& adapter)
  {
    rex::GpuDescription desc;

    // MSDN says that you shouldn't use dxgi 1.0 and dxgi 1.1
    // in the same application.
    // so if the version is 1 or higher, we use dxgi 1.1
    rex::wrl::com_ptr<IDXGIAdapter1> adapter_1;
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
    //-------------------------------------------------------------------------
    Adapter::Adapter(wrl::com_ptr<IDXGIAdapter>&& adapter, uint32 version)
        : ComObject(rsl::move(adapter), version)
        , m_description(::get_description(com_ptr()))
    {
    }

    //-------------------------------------------------------------------------
    const GpuDescription& Adapter::description() const
    {
      return m_description;
    }
  } // namespace dxgi
} // namespace rex
