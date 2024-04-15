#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/frameinfo/fps.h"
#include "rex_std/thread.h"

TEST_CASE("Fps Updating")
{
  rex::FPS fps{};

  fps.update();

  using namespace rsl::chrono_literals;
  rsl::this_thread::sleep_for(1s);

  // A few consequetive updates to increase fps
  fps.update();
  fps.update();
  fps.update();
  fps.update();
  fps.update();
  fps.update();
  fps.update();

  rsl::this_thread::sleep_for(1s);
  fps.update();

  // Above, we update 7 frames in 1 second, so our fps should be 7
  REX_CHECK(fps.get() == 7);

  rsl::this_thread::sleep_for(1s);

  fps.update();

  // above we only update 1 frame, so our fps is 1
  REX_CHECK(fps.get() == 1);
}