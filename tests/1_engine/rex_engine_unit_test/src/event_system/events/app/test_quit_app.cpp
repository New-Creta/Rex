#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/event_system/events/app/quit_app.h"
#include "rex_engine/event_system/event_system.h"

TEST_CASE("Quit App Event")
{
  s32 num_quit_events_fired = 0;
  rsl::string_view quit_msg;
  s32 exit_code = 0;
  auto subscription = rex::event_system().subscribe<rex::QuitApp>([&num_quit_events_fired, &exit_code, &quit_msg](const rex::QuitApp& evt)
    {
      ++num_quit_events_fired;
      REX_CHECK(evt.reason() == quit_msg);
      REX_CHECK(evt.exit_code() == exit_code);
    });

  // Using default exit code
  // Immediate event fire
  quit_msg = "Test Quit Event";
  rex::event_system().fire_event(rex::QuitApp(quit_msg));
  REX_CHECK(num_quit_events_fired == 1);

  // Queued event fire
  quit_msg = "Test Quit Event 2";
  rex::event_system().enqueue_event(rex::QuitApp(quit_msg));
  REX_CHECK(num_quit_events_fired == 1);

  rex::event_system().dispatch_queued_events();
  REX_CHECK(num_quit_events_fired == 2);

  // Using custom exit code
  // Immediate event fire
  quit_msg = "Test Quit Event";
  exit_code = 1;
  rex::event_system().fire_event(rex::QuitApp(quit_msg, exit_code));
  REX_CHECK(num_quit_events_fired == 3);
  
  // Queued event fire
  quit_msg = "Test Quit Event 2";
  exit_code = 2;
  rex::event_system().enqueue_event(rex::QuitApp(quit_msg, exit_code));
  REX_CHECK(num_quit_events_fired == 3);
 
  rex::event_system().dispatch_queued_events();
  REX_CHECK(num_quit_events_fired == 4);
}