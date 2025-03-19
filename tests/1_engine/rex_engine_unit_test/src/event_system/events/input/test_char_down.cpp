#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/event_system/events/input/char_down.h"
#include "rex_engine/event_system/event_system.h"

TEST_CASE("TEST - Events - Char Down Event")
{
  s32 num_quit_events_fired = 0;
  char8 key = 'a';
  auto subscription = rex::event_system().subscribe<rex::CharDown>([&num_quit_events_fired, &key](const rex::CharDown& evt)
    {
      ++num_quit_events_fired;
      REX_CHECK(evt.key() == key);
    });

  // Immediate event fire
  rex::event_system().fire_event(rex::CharDown(key));
  REX_CHECK(num_quit_events_fired == 1);

  // Queued event fire
  key = 'b';
  rex::event_system().enqueue_event(rex::CharDown(key));
  REX_CHECK(num_quit_events_fired == 1);

  rex::event_system().dispatch_queued_events();
  REX_CHECK(num_quit_events_fired == 2);
}