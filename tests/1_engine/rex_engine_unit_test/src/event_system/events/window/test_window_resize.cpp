#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/event_system/events/window/window_resize.h"
#include "rex_engine/event_system/event_system.h"

TEST_CASE("Quit App Event")
{
  s32 num_quit_events_fired = 0;
  rsl::string_view quit_msg;
  auto subscription = rex::event_system().subscribe<rex::WindowResize>([&num_quit_events_fired, &quit_msg](const rex::WindowResize& evt)
    {
      ++num_quit_events_fired;
      REX_CHECK(evt.reason() == quit_msg);
    });

  // Immediate event fire
  quit_msg = "Test Quit Event";
  rex::event_system().fire_event(rex::WindowResize(quit_msg));
  REX_CHECK(num_quit_events_fired == 1);

  // Queued event fire
  quit_msg = "Test Quit Event 2";
  rex::event_system().enqueue_event(rex::WindowResize(quit_msg));
  REX_CHECK(num_quit_events_fired == 1);

  rex::event_system().dispatch_queued_events();
  REX_CHECK(num_quit_events_fired == 2);
}