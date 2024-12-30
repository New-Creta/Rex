#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/timing/timepoint.h"

TEST_CASE("TEST - Timepoint - Timepoint To String")
{
	const rsl::time_point current_time = rsl::current_timepoint();
	rsl::string current_time_str = rex::timepoint_str(current_time);
	rsl::vector<rsl::string_view> splitted = rsl::split(current_time_str, "_");

	REX_CHECK(rsl::stoi(splitted[0]) == current_time.date().month_day());
	REX_CHECK(rsl::stoi(splitted[1]) == current_time.date().month());
	REX_CHECK(rsl::stoi(splitted[2]) == current_time.date().year());
	REX_CHECK(rsl::stoi(splitted[3]) == current_time.time().hours());
	REX_CHECK(rsl::stoi(splitted[4]) == current_time.time().minutes());
	REX_CHECK(rsl::stoi(splitted[5]) == current_time.time().seconds());

}