#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/frameinfo/deltatime.h"
#include "rex_std/thread.h"

TEST_CASE("Deltatime Updates")
{
  rex::DeltaTime dt{};

  dt.update();

  using namespace rsl::chrono_literals;
  rsl::this_thread::sleep_for(5ms);

  dt.update();

  REX_CHECK(dt.to_seconds() > 0.005f);
  REX_CHECK(dt.to_milliseconds() > 5.0f);
}