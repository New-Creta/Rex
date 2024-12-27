#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/profiling/scoped_timer.h"

#include "rex_std/chrono.h"
#include "rex_std/thread.h"

TEST_CASE("Test - Instrumentor - Construction")
{
	rsl::string_view timer_name = "This is a test name";
	rex::ScopedTimer scoped_timer(timer_name);

	REX_CHECK(scoped_timer.name() == timer_name);
	REX_CHECK(scoped_timer.source_location().file_name() == __FILE__);
	REX_CHECK(scoped_timer.source_location().line() == 8);
}

TEST_CASE("Test - Instrumentor - Timing")
{
	rsl::string_view timer_name = "This is a test name";
	rex::ScopedTimer scoped_timer(timer_name);

	using namespace rsl::chrono_literals;

	// Make the sleep time small enough that it doesn't have a big overhead on the overal test duration
	// Keep in mind that tests are run in multiple configs, possible using different versions of the executable
	rsl::chrono::milliseconds sleep_time = 50ms;
	rsl::this_thread::sleep_for(sleep_time);

	rsl::chrono::nanoseconds ns = scoped_timer.stop();

	REX_CHECK(ns >= sleep_time);
}

