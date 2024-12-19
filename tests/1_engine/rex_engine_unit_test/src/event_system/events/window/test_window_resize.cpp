#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/event_system/events/window/window_resize.h"
#include "rex_engine/event_system/event_system.h"

TEST_CASE("Window Resize Event")
{
  s32 num_quit_events_fired = 0;
  s32 width = 0;
  s32 height = 0;
  rex::WindowResizeType resize_type = rex::WindowResizeType::Invalid;
  auto subscription = rex::event_system().subscribe<rex::WindowResize>([&num_quit_events_fired, &width, &height, &resize_type](const rex::WindowResize& evt)
    {
      ++num_quit_events_fired;
      REX_CHECK(evt.width() == width);
      REX_CHECK(evt.height() == height);
      REX_CHECK(evt.resize_type() == resize_type);
    });

  // Immediate event fire
  width = 10;
  height = 20;
  resize_type = rex::WindowResizeType::Maximized;
  rex::event_system().fire_event(rex::WindowResize(width, height, resize_type));
  REX_CHECK(num_quit_events_fired == 1);

  // Queued event fire
  width = 10;
  height = 20;
  resize_type = rex::WindowResizeType::Maximized;
  rex::event_system().enqueue_event(rex::WindowResize(width, height, resize_type));
  REX_CHECK(num_quit_events_fired == 1);

  rex::event_system().dispatch_queued_events();
  REX_CHECK(num_quit_events_fired == 2);
}