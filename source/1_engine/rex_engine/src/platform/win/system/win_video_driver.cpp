#include "rex_engine/platform/win/system/win_video_driver.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/platform/win/system/win_registry.h"
#include "rex_engine/text_processing/text_processing.h"

#include "rex_std/string.h"
#include "rex_std/array.h"
#include "rex_std/bonus/memory.h"
#include "rex_std/bonus/utility.h"

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

	bool get_device_description(HDEVINFO devInfoHandle, SP_DEVINFO_DATA& deviceInfoData, rsl::Out<rsl::wbig_stack_string> out)
	{
		DWORD data_type = 0;
		bool res = SetupDiGetDevicePropertyW(devInfoHandle, &deviceInfoData, &DEVPKEY_Device_DriverDesc, &data_type, (PBYTE)out.get().data(), out.get().max_size(), nullptr, 0);
		out.get().reset_null_termination_offset();

		return res;
	}

	bool get_device_driver_reg_key(HDEVINFO devInfoHandle, SP_DEVINFO_DATA& deviceInfoData, rsl::Out<rsl::wbig_stack_string> out)
	{
		DWORD data_type = 0;
		bool res = SetupDiGetDevicePropertyW(devInfoHandle, &deviceInfoData, &DEVPKEY_Device_Driver, &data_type, (PBYTE)out.get().data(), out.get().max_size(), nullptr, 0);
		out.get().reset_null_termination_offset();

		return res;
	}

	bool get_provider_name(HDEVINFO devInfoHandle, SP_DEVINFO_DATA& deviceInfoData, rsl::Out<rsl::wbig_stack_string> out)
	{
		DWORD data_type = 0;
		bool res = SetupDiGetDevicePropertyW(devInfoHandle, &deviceInfoData, &DEVPKEY_Device_DriverProvider, &data_type, (PBYTE)out.get().data(), out.get().max_size(), nullptr, 0);
		out.get().reset_null_termination_offset();

		return res;
	}

	bool get_internal_driver_version(HDEVINFO devInfoHandle, SP_DEVINFO_DATA& deviceInfoData, rsl::Out<rsl::wbig_stack_string> out)
	{
		DWORD data_type = 0;
		bool res = SetupDiGetDevicePropertyW(devInfoHandle, &deviceInfoData, &DEVPKEY_Device_DriverVersion, &data_type, (PBYTE)out.get().data(), out.get().max_size(), nullptr, 0);
		out.get().reset_null_termination_offset();

		return res;
	}

	bool get_driver_date(HDEVINFO devInfoHandle, SP_DEVINFO_DATA& deviceInfoData, rsl::Out<SYSTEMTIME> out)
	{
		DWORD data_type = 0;
		FILETIME file_time;
		bool res = SetupDiGetDevicePropertyW(devInfoHandle, &deviceInfoData, &DEVPKEY_Device_DriverDate, &data_type, (PBYTE)&file_time, sizeof(FILETIME), nullptr, 0);
		FileTimeToSystemTime(&file_time, &out.get());

		return res;
	}

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
		bool bFound = false;
		SP_DEVINFO_DATA device_info_data;
		device_info_data.cbSize = sizeof(SP_DEVINFO_DATA);

		for (int32 idx = 0; SetupDiEnumDeviceInfo(dev_info_handle, idx, &device_info_data); idx++)
		{
			if (get_device_description(dev_info_handle, device_info_data, rsl::Out(buffer)))
			{
				if (wide_device_name == buffer.to_view())
				{
					driver_info.device_description = rsl::to_string(buffer);
					bFound = true;
					buffer.clear();

					// Retrieve the registry key for this device for 3rd party data
					if (get_device_driver_reg_key(dev_info_handle, device_info_data, rsl::Out(buffer)))
					{
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
		if (get_provider_name(dev_info_handle, device_info_data, rsl::Out(buffer)))
		{
			driver_info.provider_name = rsl::to_string(buffer);
			buffer.clear();
		}
		else
		{
			REX_INFO(LogVideoDriver, "Failed to find provider name");
		}

		// Get the internal driver version
		if (get_internal_driver_version(dev_info_handle, device_info_data, rsl::Out(buffer)))
		{
			driver_info.internal_driver_version = rsl::to_string(buffer);
			buffer.clear();
		}
		else
		{
			REX_INFO(LogVideoDriver, "Failed to find internal driver version");
		}

		// Get the driver date
		SYSTEMTIME system_time;
		if (get_driver_date(dev_info_handle, device_info_data, rsl::Out(system_time)))
		{
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