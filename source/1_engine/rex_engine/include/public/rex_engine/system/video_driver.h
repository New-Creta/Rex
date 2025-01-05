#pragma once

#include "rex_std/string_view.h"

namespace rex
{
  //-------------------------------------------------------------------------
	/**
	 * The VendorId is a unique identifier assigned by the PCI-SIG (Peripheral Component Interconnect Special Interest Group)
	 * to identify the manufacturer of a PCI device, including graphics adapters.
	 * The VendorId is a 16-bit unsigned integer that is typically included in
	 * the PCI Configuration space of the device.
	 */
  enum class Vendor
  {
    Unknown = 0,
    Amd = 0x1002,
    Nvidia = 0x10DE,
    Intel = 0x163C
  };

	struct VideoDriverInfo
	{
		rsl::string device_description;
		rsl::string provider_name;
		rsl::string internal_driver_version;
		rsl::string user_driver_version;
		rsl::string driver_date;
		Vendor vendor_id = Vendor::Unknown;
	};

	VideoDriverInfo query_video_driver(rsl::string_view deviceName);
	rsl::string nvidia_unified_version(rsl::string_view internalVersion);
}

#ifdef REX_PLATFORM_WINDOWS
#include "rex_engine/platform/win/system/win_video_driver.h"
#endif