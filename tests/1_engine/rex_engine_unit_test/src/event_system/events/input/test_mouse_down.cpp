#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/event_system/events/input/mouse_down.h"
#include "rex_engine/event_system/event_system.h"

TEST_CASE("TEST - Events - Mouse Down Event")
{
  s32 num_quit_events_fired = 0;
  rex::MouseButton mouse_button = rex::MouseButton::Unknown;
  rex::MousePosition mouse_pos{};
  rex::IsDoubleClick is_double_click = rex::IsDoubleClick::no;

  auto subscription = rex::event_system().subscribe<rex::MouseDown>([&num_quit_events_fired, &mouse_button, &mouse_pos, &is_double_click](const rex::MouseDown& evt)
    {
      ++num_quit_events_fired;
      REX_CHECK(evt.mouse_button() == mouse_button);
      REX_CHECK(evt.mouse_position().local_pos == mouse_pos.local_pos);
      REX_CHECK(evt.mouse_position().screen_pos == mouse_pos.screen_pos);
      REX_CHECK(evt.is_double_click() == is_double_click);
    });

  // Immediate event fire
  mouse_button = rex::MouseButton::Left;
  mouse_pos.local_pos = rsl::pointi32{ 10, 10 };
  mouse_pos.screen_pos = rsl::pointi32{ 20, 20 };
  is_double_click = rex::IsDoubleClick::yes;
  rex::event_system().fire_event(rex::MouseDown(mouse_button, mouse_pos, is_double_click));
  REX_CHECK(num_quit_events_fired == 1);

  // Queued event fire
  mouse_button = rex::MouseButton::Right;
  mouse_pos.local_pos = rsl::pointi32{ 40, 40 };
  mouse_pos.screen_pos = rsl::pointi32{ 50, 50 };
  is_double_click = rex::IsDoubleClick::no;
  rex::event_system().enqueue_event(rex::MouseDown(mouse_button, mouse_pos, is_double_click));
  REX_CHECK(num_quit_events_fired == 1);

  rex::event_system().dispatch_queued_events();
  REX_CHECK(num_quit_events_fired == 2);
}