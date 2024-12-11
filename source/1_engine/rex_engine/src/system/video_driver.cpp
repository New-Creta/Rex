#include "rex_engine/system/video_driver.h"

namespace rex
{
	rsl::string nvidia_unified_version(rsl::string_view internalVersion)
	{
		// Ignore the Windows/DirectX version by taking the last digits of the internal version
		// and moving the version dot. Coincidentally, that's the user-facing string. For example:
		// 9.18.13.4788 -> 3.4788 -> 347.88
		if (internalVersion.length() < 6)
		{
			return rsl::string(internalVersion);
		}

		rsl::string unified_version(internalVersion.substr(6));
		unified_version.replace(".", "");
		unified_version.insert(3, ".");

		return unified_version;
	}
}