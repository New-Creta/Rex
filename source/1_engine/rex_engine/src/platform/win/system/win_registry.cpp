#include "rex_engine/platform/win/system/win_registry.h"

#include "rex_std/memory.h"

#include <Windows.h>

namespace rex
{
	namespace win
	{
		namespace registry
		{
			rsl::optional<rsl::string> query_key(HKEY hkey, rsl::string_view subKey, rsl::string_view valueName)
			{
				// Redirect key depending on system
				for (int32 registry_idx = 0; registry_idx < 2; ++registry_idx)
				{
					HKEY key = 0;
					const uint32 reg_flags = (registry_idx == 0) ? KEY_WOW64_32KEY : KEY_WOW64_64KEY;
					if (RegOpenKeyEx(hkey, subKey.data(), 0, KEY_READ | reg_flags, &key) == ERROR_SUCCESS)
					{
						::DWORD size = 0, value_type = 0;
						// First, we'll call RegQueryValueEx to find out how large of a buffer we need
						if ((RegQueryValueEx(key, valueName.data(), NULL, &value_type, NULL, &size) == ERROR_SUCCESS) && size)
						{
							switch (value_type)
							{
							case REG_DWORD:
							{
								::DWORD value;
								if (RegQueryValueEx(key, valueName.data(), NULL, NULL, (LPBYTE)&value, &size) == ERROR_SUCCESS)
								{
									return rsl::to_string(value);
								}
								break;
							}

							case REG_QWORD:
							{
								int64 value;
								if (RegQueryValueEx(key, valueName.data(), NULL, NULL, (LPBYTE)&value, &size) == ERROR_SUCCESS)
								{
									return rsl::to_string(value);
								}
								break;
							}

							case REG_SZ:
							case REG_EXPAND_SZ:
							case REG_MULTI_SZ:
							{
								// Allocate a buffer to hold the value and call the function again to get the data
								rsl::unique_array<char> buffer = rsl::make_unique<char[]>(size);
								if (RegQueryValueEx(key, valueName.data(), NULL, NULL, (LPBYTE)buffer.get(), &size) == ERROR_SUCCESS)
								{
									const uint32 length = (size / sizeof(TCHAR)) - 1;
									return rsl::string(buffer.get(), buffer.count());
								}
								break;
							}
							}
						}
						RegCloseKey(key);
					}
				}

				return rsl::nullopt;
			}
		}
	}
}