#include "rex_engine/system/video_driver.h"

#include "rex_std/algorithm.h"

namespace rex
{
	//-------------------------------------------------------------------------
	rsl::small_stack_string vendor_to_string(s32 vendor)
	{
		// Enum reflection is not possible here as the integer values are
		// outside the valid range of values [0, 127] for this enumeration type
		switch (static_cast<rex::Vendor>(vendor))
		{
		case rex::Vendor::Amd: return rsl::small_stack_string("AMD");
		case rex::Vendor::Nvidia: return rsl::small_stack_string("NVIDIA");
		case rex::Vendor::Intel: return rsl::small_stack_string("INTEL");
		default: return rsl::small_stack_string("Unknown Vendor");
		}
	}

	rsl::string nvidia_unified_version(rsl::string_view internalVersion)
	{
		// Ignore the Windows/DirectX version by taking the last digits of the internal version
		// and moving the version dot. Coincidentally, that's the user-facing string. For example:
		// 9.18.13.4788 -> 3.4788 -> 347.88
		if (internalVersion.length() < 6)
		{
			return rsl::string(internalVersion);
		}

		if (rsl::any_of(internalVersion.cbegin(), internalVersion.cend(), [](char8 c) { return !rsl::is_digit(c) && c != '.'; }))
		{
			return rsl::string(internalVersion);
		}

		rsl::string unified_version(internalVersion.substr(6));
		unified_version.replace(".", "");
		unified_version.insert(3, ".");

		return unified_version;
	}
}