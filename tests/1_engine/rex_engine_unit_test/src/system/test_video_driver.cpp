#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/system/video_driver.h"

TEST_CASE("TEST - Video Driver - Querying")
{
	rex::VideoDriverInfo video_driver_info = rex::query_video_driver("");

	// If no valid gpu name is given, driver info should be empty
	REX_CHECK(video_driver_info.device_description == "");
	REX_CHECK(video_driver_info.provider_name == "");
	REX_CHECK(video_driver_info.internal_driver_version == "");
	REX_CHECK(video_driver_info.user_driver_version == "");
	REX_CHECK(video_driver_info.driver_date == "");
	REX_CHECK(video_driver_info.vendor_id == rex::Vendor::Unknown);

	// I'm not sure how to test this with a valid gpu name as it requires engine initialization
	// which takes some time and has some dependencies
}
TEST_CASE("TEST - Video Driver - Nvidia Unified Version")
{
	// Invalid version 1
	rsl::string_view invalid_version1 = "this is some invalid version";
	rsl::string invalid_nvidia_version1 = rex::nvidia_unified_version(invalid_version1);
	REX_CHECK(invalid_nvidia_version1 == invalid_version1);

	// Invalid version 2
	rsl::string_view invalid_version2 = "";
	rsl::string invalid_nvidia_version2 = rex::nvidia_unified_version(invalid_version2);
	REX_CHECK(invalid_nvidia_version2 == invalid_version2);

	// Invalid version 3
	rsl::string_view invalid_version3 = "short";
	rsl::string invalid_nvidia_version3 = rex::nvidia_unified_version(invalid_version3);
	REX_CHECK(invalid_nvidia_version3 == invalid_version3);

	// Valid version
	rsl::string_view version = "1.23.45.6789";
	rsl::string nvidia_version = rex::nvidia_unified_version(version);

	REX_CHECK(nvidia_version == "567.89");
}