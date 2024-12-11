#include "rex_engine/platform/win/system/win_video_driver.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/platform/win/system/win_registry.h"
#include "rex_engine/text_processing/text_processing.h"

#include "rex_std/string.h"
#include "rex_std/array.h"
#include "rex_std/bonus/memory.h"

#define USE_SP_ALTPLATFORM_INFO_V1 0
#define USE_SP_ALTPLATFORM_INFO_V3 1
#define USE_SP_DRVINFO_DATA_V1 0
#define USE_SP_BACKUP_QUEUE_PARAMS_V1 0
#define USE_SP_INF_SIGNER_INFO_V1 0
#include <Windows.h>
#include <SetupAPI.h>
#include <initguid.h>
#include <devguid.h>
#include <devpkey.h>
#undef USE_SP_ALTPLATFORM_INFO_V1
#undef USE_SP_ALTPLATFORM_INFO_V3
#undef USE_SP_DRVINFO_DATA_V1
#undef USE_SP_BACKUP_QUEUE_PARAMS_V1
#undef USE_SP_INF_SIGNER_INFO_V1

namespace rex
{
	DEFINE_LOG_CATEGORY(LogVideoDriver);

	VideoDriverInfo query_video_driver(rsl::string_view deviceName)
	{
		VideoDriverInfo driver_info{};

		HDEVINFO dev_info_handle = SetupDiGetClassDevs(&GUID_DEVCLASS_DISPLAY, NULL, NULL, DIGCF_PRESENT);

		rsl::wstring registry_key;

		rsl::wstring wide_device_name = rsl::to_wstring(deviceName);

		if (dev_info_handle == INVALID_HANDLE_VALUE)
		{
			REX_INFO(LogVideoDriver, "Failed to initialize Setup API");
			return driver_info;
		}

		rsl::wbig_stack_string buffer;
		DWORD data_type = 0;
		bool bFound = false;
		SP_DEVINFO_DATA device_info_data;
		device_info_data.cbSize = sizeof(SP_DEVINFO_DATA);

		for (int32 idx = 0; SetupDiEnumDeviceInfo(dev_info_handle, idx, &device_info_data); idx++)
		{
			// Get the device description, check if it matches the queried device name
			if (SetupDiGetDevicePropertyW(dev_info_handle, &device_info_data, &DEVPKEY_Device_DriverDesc, &data_type,
				(PBYTE)buffer.data(), buffer.max_size(), nullptr, 0))
			{
				buffer.reset_null_termination_offset();
				if (wide_device_name == buffer.to_view())
				{
					driver_info.device_description = rsl::to_string(buffer);
					bFound = true;
					buffer.clear();

					// Retrieve the registry key for this device for 3rd party data
					if (SetupDiGetDevicePropertyW(dev_info_handle, &device_info_data, &DEVPKEY_Device_Driver, &data_type,
						(PBYTE)buffer.data(), buffer.max_size(), nullptr, 0))
					{
						buffer.reset_null_termination_offset();
						const tchar* ptr = buffer.data();
						registry_key.assign(buffer.data(), buffer.size());
						buffer.clear();
					}
					else
					{
						REX_INFO(LogVideoDriver, "Failed to retrieve driver registry key for device {}", idx);
					}

					break;
				}
				buffer.clear();
			}
			else
			{
				REX_INFO(LogVideoDriver, "Failed to retrieve driver description for device {}", idx);
			}
		}

		if (!bFound)
		{
			REX_INFO(LogVideoDriver, "Unable to find requested device '{}' using Setup API.", deviceName);
			return driver_info;
		}

		// Get the provider name
		if (SetupDiGetDevicePropertyW(dev_info_handle, &device_info_data, &DEVPKEY_Device_DriverProvider, &data_type,
			(PBYTE)buffer.data(), buffer.max_size(), nullptr, 0))
		{
			buffer.reset_null_termination_offset();
			driver_info.provider_name = rsl::to_string(buffer);
			buffer.clear();
		}
		else
		{
			REX_INFO(LogVideoDriver, "Failed to find provider name");
		}
		// Get the internal driver version
		if (SetupDiGetDevicePropertyW(dev_info_handle, &device_info_data, &DEVPKEY_Device_DriverVersion, &data_type,
			(PBYTE)buffer.data(), buffer.max_size(), nullptr, 0))
		{
			buffer.reset_null_termination_offset();
			driver_info.internal_driver_version = rsl::to_string(buffer);
			buffer.clear();
		}
		else
		{
			REX_INFO(LogVideoDriver, "Failed to find internal driver version");
		}
		// Get the driver date
		FILETIME file_time;
		if (SetupDiGetDevicePropertyW(dev_info_handle, &device_info_data, &DEVPKEY_Device_DriverDate, &data_type,
			(PBYTE)&file_time, sizeof(FILETIME), nullptr, 0))
		{
			SYSTEMTIME system_time;
			FileTimeToSystemTime(&file_time, &system_time);
			driver_info.driver_date.assign(rsl::format("{} {} {}", system_time.wDay, rex::month_nr_to_name(system_time.wMonth), system_time.wYear));
		}
		else
		{
			REX_INFO(LogVideoDriver, "Failed to find driver date");
		}

		SetupDiDestroyDeviceInfoList(dev_info_handle);

		if (!driver_info.provider_name.empty())
		{
			if (driver_info.provider_name.contains("NVIDIA"))
			{
				driver_info.vendor_id = Vendor::Nvidia;
			}
			else if (driver_info.provider_name.contains("Advanced Micro Devices"))
			{
				driver_info.vendor_id = Vendor::Amd;
			}
			else if (driver_info.provider_name.contains("Intel"))	// usually TEXT("Intel Corporation")
			{
				driver_info.vendor_id = Vendor::Intel;
			}
			else
			{
				driver_info.vendor_id = Vendor::Unknown;
			}
		}

		driver_info.user_driver_version = driver_info.internal_driver_version;

		if (driver_info.vendor_id == Vendor::Nvidia)
		{
			driver_info.user_driver_version = nvidia_unified_version(driver_info.internal_driver_version);
		}
		else if (driver_info.vendor_id == Vendor::Amd && !registry_key.empty())
		{
			// Get the AMD specific information directly from the registry
			// AMD AGS could be used instead, but retrieving the radeon software version cannot occur after a D3D Device
			// has been created, and this function could be called at any time

			rsl::string narrow_registry_key = rsl::to_string(registry_key);
			const rsl::string key(rsl::format("SYSTEM\\CurrentControlSet\\Control\\Class\\{}", narrow_registry_key));
			auto user_driver_version = win::registry::query_key(HKEY_LOCAL_MACHINE, key, "Catalyst_Version");
			if (user_driver_version.has_value())
			{
				driver_info.user_driver_version.assign(rsl::format("Catalyst {}", user_driver_version.value()));
			}

			auto edition = win::registry::query_key(HKEY_LOCAL_MACHINE, key, "RadeonSoftwareEdition");
			if (edition.has_value())
			{
				auto version = win::registry::query_key(HKEY_LOCAL_MACHINE, key, "RadeonSoftwareVersion");
				if (version.has_value())
				{
					// e.g. TEXT("Crimson 15.12") or TEXT("Catalyst 14.1")
					driver_info.user_driver_version.assign(rsl::format("{} {}", edition.value(), version.value()));
				}
			}
		}

		return driver_info;
	}
}