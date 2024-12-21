#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/event_system/events/input/mouse_scroll.h"
#include "rex_engine/event_system/event_system.h"

TEST_CASE("TEST - Events - Mouse Scroll Event")
{
  s32 num_quit_events_fired = 0;
  s32 wheel_delta = 0;
  rex::MousePosition mouse_pos;
  auto subscription = rex::event_system().subscribe<rex::MouseScroll>([&num_quit_events_fired, &wheel_delta, &mouse_pos](const rex::MouseScroll& evt)
    {
      ++num_quit_events_fired;
      REX_CHECK(evt.wheel_delta() == wheel_delta);
      REX_CHECK(evt.mouse_position().local_pos == mouse_pos.local_pos);
      REX_CHECK(evt.mouse_position().screen_pos == mouse_pos.screen_pos);
    });

  // Immediate event fire
  wheel_delta = 10;
  mouse_pos.local_pos = { 10,10 };
  mouse_pos.screen_pos = { 20,20 };
  rex::event_system().fire_event(rex::MouseScroll(wheel_delta, mouse_pos));
  REX_CHECK(num_quit_events_fired == 1);

  // Queued event fire
  wheel_delta = 10;
  mouse_pos.local_pos = { 10,10 };
  mouse_pos.screen_pos = { 20,20 };
  rex::event_system().enqueue_event(rex::MouseScroll(wheel_delta, mouse_pos));
  REX_CHECK(num_quit_events_fired == 1);

  rex::event_system().dispatch_queued_events();
  REX_CHECK(num_quit_events_fired == 2);
}