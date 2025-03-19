#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/timing/interval.h"

#include "rex_std/thread.h"

TEST_CASE("TEST - Interval - Timing")
{
  rex::Interval interval{};

  using namespace rsl::chrono_literals;
  rsl::this_thread::sleep_for(1ms);

  REX_CHECK(interval.milliseconds() > 0);
  REX_CHECK(interval.seconds() > 0);
}

TEST_CASE("TEST - Interval - Resetting")
{
  rex::Interval interval{};

  using namespace rsl::chrono_literals;
  rsl::this_thread::sleep_for(1ms);

  interval.reset();
  REX_CHECK(interval.milliseconds() == 0);
  REX_CHECK(interval.seconds() == 0);
}