#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/engine/state_controller.h"
#include "rex_engine/engine/defines.h"

namespace rex::test
{
  enum class TestEnum
  {
    FirstBit = BIT(0),
    SecondBit = BIT(1),
    ThirdBit = BIT(2)
  };
}

TEST_CASE("StateController Default Construction")
{
  rex::StateController<rex::test::TestEnum> state_controller;

  REX_CHECK(state_controller.has_state(rex::test::TestEnum::FirstBit) == false);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::SecondBit) == false);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::ThirdBit) == false);
}

TEST_CASE("StateController Value Construction")
{
  {
    rex::StateController<rex::test::TestEnum> state_controller(rex::test::TestEnum::FirstBit);
    REX_CHECK(state_controller.has_state(rex::test::TestEnum::FirstBit) == true);
    REX_CHECK(state_controller.has_state(rex::test::TestEnum::SecondBit) == false);
    REX_CHECK(state_controller.has_state(rex::test::TestEnum::ThirdBit) == false);
  }

  {
    rex::StateController<rex::test::TestEnum> state_controller(rex::test::TestEnum::SecondBit);
    REX_CHECK(state_controller.has_state(rex::test::TestEnum::FirstBit) == false);
    REX_CHECK(state_controller.has_state(rex::test::TestEnum::SecondBit) == true);
    REX_CHECK(state_controller.has_state(rex::test::TestEnum::ThirdBit) == false);
  }

  {
    rex::StateController<rex::test::TestEnum> state_controller(rex::test::TestEnum::ThirdBit);
    REX_CHECK(state_controller.has_state(rex::test::TestEnum::FirstBit) == false);
    REX_CHECK(state_controller.has_state(rex::test::TestEnum::SecondBit) == false);
    REX_CHECK(state_controller.has_state(rex::test::TestEnum::ThirdBit) == true);
  }
}

TEST_CASE("StateController Changing State")
{
  rex::StateController<rex::test::TestEnum> state_controller;

  state_controller.change_state(rex::test::TestEnum::FirstBit);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::FirstBit) == true);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::SecondBit) == false);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::ThirdBit) == false);

  state_controller.change_state(rex::test::TestEnum::SecondBit);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::FirstBit) == false);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::SecondBit) == true);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::ThirdBit) == false);

  state_controller.change_state(rex::test::TestEnum::ThirdBit);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::FirstBit) == false);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::SecondBit) == false);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::ThirdBit) == true);
}

TEST_CASE("StateController Adding State")
{
  rex::StateController<rex::test::TestEnum> state_controller;

  state_controller.add_state(rex::test::TestEnum::FirstBit);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::FirstBit) == true);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::SecondBit) == false);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::ThirdBit) == false);

  state_controller.add_state(rex::test::TestEnum::SecondBit);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::FirstBit) == true);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::SecondBit) == true);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::ThirdBit) == false);

  state_controller.add_state(rex::test::TestEnum::ThirdBit);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::FirstBit) == true);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::SecondBit) == true);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::ThirdBit) == true);
}

TEST_CASE("StateController Removing State")
{
  rex::StateController<rex::test::TestEnum> state_controller(static_cast<rex::test::TestEnum>(0b00000111));

  state_controller.remove_state(rex::test::TestEnum::FirstBit);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::FirstBit) == false);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::SecondBit) == true);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::ThirdBit) == true);

  state_controller.remove_state(rex::test::TestEnum::SecondBit);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::FirstBit) == false);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::SecondBit) == false);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::ThirdBit) == true);

  state_controller.remove_state(rex::test::TestEnum::ThirdBit);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::FirstBit) == false);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::SecondBit) == false);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::ThirdBit) == false);
}

TEST_CASE("StateController Integration Tests")
{
  rex::StateController<rex::test::TestEnum> state_controller;

  state_controller.add_state(rex::test::TestEnum::FirstBit);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::FirstBit) == true);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::SecondBit) == false);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::ThirdBit) == false);

  state_controller.change_state(rex::test::TestEnum::FirstBit);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::FirstBit) == true);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::SecondBit) == false);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::ThirdBit) == false);

  state_controller.change_state(rex::test::TestEnum::SecondBit);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::FirstBit) == false);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::SecondBit) == true);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::ThirdBit) == false);

  state_controller.add_state(rex::test::TestEnum::FirstBit);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::FirstBit) == true);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::SecondBit) == true);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::ThirdBit) == false);

  state_controller.remove_state(rex::test::TestEnum::ThirdBit);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::FirstBit) == true);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::SecondBit) == true);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::ThirdBit) == false);

  state_controller.remove_state(rex::test::TestEnum::SecondBit);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::FirstBit) == true);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::SecondBit) == false);
  REX_CHECK(state_controller.has_state(rex::test::TestEnum::ThirdBit) == false);
}