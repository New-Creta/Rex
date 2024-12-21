#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/event_system/events/input/mouse_up.h"
#include "rex_engine/event_system/event_system.h"

TEST_CASE("TEST - Events - Mouse Up Event")
{
  s32 num_quit_events_fired = 0;
  rex::MouseButton mouse_button = rex::MouseButton::Left;
  rex::MousePosition mouse_pos;
  auto subscription = rex::event_system().subscribe<rex::MouseUp>([&num_quit_events_fired, &mouse_button, &mouse_pos](const rex::MouseUp& evt)
    {
      ++num_quit_events_fired;
      REX_CHECK(evt.mouse_button() == mouse_button);
      REX_CHECK(evt.mouse_position().local_pos == mouse_pos.local_pos);
      REX_CHECK(evt.mouse_position().screen_pos == mouse_pos.screen_pos);
    });

  // Immediate event fire
  mouse_button = rex::MouseButton::Right;
  mouse_pos.local_pos = { 10,10 };
  mouse_pos.screen_pos = { 20,20 };
  rex::event_system().fire_event(rex::MouseUp(mouse_button, mouse_pos));
  REX_CHECK(num_quit_events_fired == 1);

  // Queued event fire
  mouse_button = rex::MouseButton::Left;
  mouse_pos.local_pos = { 30,30 };
  mouse_pos.screen_pos = { 40,40 };
  rex::event_system().enqueue_event(rex::MouseUp(mouse_button, mouse_pos));
  REX_CHECK(num_quit_events_fired == 1);

  rex::event_system().dispatch_queued_events();
  REX_CHECK(num_quit_events_fired == 2);
}