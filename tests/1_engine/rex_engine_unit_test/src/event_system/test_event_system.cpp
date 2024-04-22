#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/event_system/event_system.h"

#include "rex_engine/event_system/events/input/char_down.h"
#include "rex_engine/event_system/events/window/window_resize.h"

#include <random>

TEST_CASE("EventSystem - check removal of subscriptions")
{
  s32 x = 0;
  auto inc_x = [&x](const rex::CharDown& /*evt*/) { ++x; };
  auto subscription = rex::event_system().subscribe<rex::CharDown>(inc_x);

  REX_CHECK(x == 0);

  rex::event_system().fire_event(rex::CharDown('a'));
  REX_CHECK(x == 1);

  rex::event_system().remove_subscription(subscription);

  rex::event_system().fire_event(rex::CharDown('a'));
  REX_CHECK(x == 1);
}

TEST_CASE("EventSystem - queueing events")
{
  s32 x = 0;
  auto inc_x = [&x](const rex::CharDown& /*evt*/) { ++x; };
  auto subscription = rex::event_system().subscribe<rex::CharDown>(inc_x);

  REX_CHECK(x == 0);

  rex::event_system().enqueue_event(rex::CharDown('a'));

  REX_CHECK(x == 0);

  rex::event_system().dispatch_queued_events();

  REX_CHECK(x == 1);

  x = 0;
  rex::event_system().enqueue_event(rex::CharDown('a'));
  rex::event_system().enqueue_event(rex::CharDown('a'));
  REX_CHECK(x == 0);

  rex::event_system().dispatch_queued_events();
  REX_CHECK(x == 2);

  rex::event_system().remove_subscription(subscription);
}

TEST_CASE("EventSystem - immediate firing events")
{
  s32 x = 0;
  auto inc_x = [&x](const rex::CharDown& /*evt*/)
  {
    ++x;
  };
  auto subscription = rex::event_system().subscribe<rex::CharDown>(inc_x);

  REX_CHECK(x == 0);

  rex::event_system().fire_event(rex::CharDown('a'));

  REX_CHECK(x == 1);

  rex::event_system().dispatch_queued_events();

  REX_CHECK(x == 1);

  x = 0;
  rex::event_system().fire_event(rex::CharDown('a'));
  rex::event_system().fire_event(rex::CharDown('a'));
  REX_CHECK(x == 2);

  rex::event_system().dispatch_queued_events();
  REX_CHECK(x == 2);

  rex::event_system().remove_subscription(subscription);
}

TEST_CASE("EventSystem - immediate firing and queing")
{
  s32 x = 0;
  auto inc_x = [&x](const rex::CharDown& /*evt*/) { ++x; };
  auto subscription = rex::event_system().subscribe<rex::CharDown>(inc_x);

  REX_CHECK(x == 0);

  rex::event_system().fire_event(rex::CharDown('a'));
  rex::event_system().enqueue_event(rex::CharDown('a'));

  REX_CHECK(x == 1);

  rex::event_system().dispatch_queued_events();

  REX_CHECK(x == 2);

  x = 0;
  rex::event_system().fire_event(rex::CharDown('a'));
  rex::event_system().fire_event(rex::CharDown('a'));
  rex::event_system().enqueue_event(rex::CharDown('a'));
  rex::event_system().enqueue_event(rex::CharDown('a'));
  REX_CHECK(x == 2);

  rex::event_system().dispatch_queued_events();
  REX_CHECK(x == 4);

  rex::event_system().remove_subscription(subscription);
}

TEST_CASE("EventSystem - test against data corruption")
{
  s32 width = 0;
  s32 height = 0;
  auto subscription = rex::event_system().subscribe<rex::WindowResize>(
    [&width, &height](const rex::WindowResize& evt)
    {
      REX_CHECK(evt.width() == width);
      REX_CHECK(evt.height() == height);
    });

  for (s32 i = 0; i < 1000; ++i)
  {
    width = std::rand();
    height = std::rand();

    rex::event_system().enqueue_event(rex::WindowResize(width, height, rex::WindowResizeType::Maximized));
    rex::event_system().fire_event(rex::WindowResize(width, height, rex::WindowResizeType::Maximized));
    rex::event_system().dispatch_queued_events();
  }

  rex::event_system().remove_subscription(subscription);
}
