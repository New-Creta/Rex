// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: test_object_test.cpp
// Copyright (c) Nick De Breuck 2023
//
// ============================================

#include "rex_std/internal/utility/move.h"

#include "rex_unit_test/rex_catch2.h"

#include "rex_unit_test/test_object.h"

TEST_CASE("test object")
{
  {
    rex::test::test_object::reset();

    rex::test::test_object obj{};

    CHECK(rex::test::test_object::num_created() == 1);
    CHECK(rex::test::test_object::num_ctor_calls() == 1);
    CHECK(rex::test::test_object::num_dtor_calls() == 0);
    CHECK(rex::test::test_object::num_copy_ctor_calls() == 0);
    CHECK(rex::test::test_object::num_move_ctor_calls() == 0);
    CHECK(rex::test::test_object::num_copy_assignment_calls() == 0);
    CHECK(rex::test::test_object::num_move_assignment_calls() == 0);
  }

  CHECK(rex::test::test_object::num_created() == 1);
  CHECK(rex::test::test_object::num_ctor_calls() == 1);
  CHECK(rex::test::test_object::num_dtor_calls() == 1);
  CHECK(rex::test::test_object::num_copy_ctor_calls() == 0);
  CHECK(rex::test::test_object::num_move_ctor_calls() == 0);
  CHECK(rex::test::test_object::num_copy_assignment_calls() == 0);
  CHECK(rex::test::test_object::num_move_assignment_calls() == 0);

  {
    rex::test::test_object::reset();

    rex::test::test_object obj{};
    rex::test::test_object obj2{ obj };

    CHECK(rex::test::test_object::num_created() == 2);
    CHECK(rex::test::test_object::num_ctor_calls() == 1);
    CHECK(rex::test::test_object::num_dtor_calls() == 0);
    CHECK(rex::test::test_object::num_copy_ctor_calls() == 1);
    CHECK(rex::test::test_object::num_move_ctor_calls() == 0);
    CHECK(rex::test::test_object::num_copy_assignment_calls() == 0);
    CHECK(rex::test::test_object::num_move_assignment_calls() == 0);
  }

  CHECK(rex::test::test_object::num_created() == 2);
  CHECK(rex::test::test_object::num_ctor_calls() == 1);
  CHECK(rex::test::test_object::num_dtor_calls() == 2);
  CHECK(rex::test::test_object::num_copy_ctor_calls() == 1);
  CHECK(rex::test::test_object::num_move_ctor_calls() == 0);
  CHECK(rex::test::test_object::num_copy_assignment_calls() == 0);
  CHECK(rex::test::test_object::num_move_assignment_calls() == 0);

  {
    rex::test::test_object::reset();

    rex::test::test_object obj{};
    rex::test::test_object obj2{ rsl::move(obj) };

    CHECK(rex::test::test_object::num_created() == 2);
    CHECK(rex::test::test_object::num_ctor_calls() == 1);
    CHECK(rex::test::test_object::num_dtor_calls() == 0);
    CHECK(rex::test::test_object::num_copy_ctor_calls() == 0);
    CHECK(rex::test::test_object::num_move_ctor_calls() == 1);
    CHECK(rex::test::test_object::num_copy_assignment_calls() == 0);
    CHECK(rex::test::test_object::num_move_assignment_calls() == 0);
  }

  CHECK(rex::test::test_object::num_created() == 2);
  CHECK(rex::test::test_object::num_ctor_calls() == 1);
  CHECK(rex::test::test_object::num_dtor_calls() == 2);
  CHECK(rex::test::test_object::num_copy_ctor_calls() == 0);
  CHECK(rex::test::test_object::num_move_ctor_calls() == 1);
  CHECK(rex::test::test_object::num_copy_assignment_calls() == 0);
  CHECK(rex::test::test_object::num_move_assignment_calls() == 0);

  {
    rex::test::test_object::reset();

    rex::test::test_object obj{};
    rex::test::test_object obj2{};
    obj2 = obj;

    CHECK(rex::test::test_object::num_created() == 2);
    CHECK(rex::test::test_object::num_ctor_calls() == 2);
    CHECK(rex::test::test_object::num_dtor_calls() == 0);
    CHECK(rex::test::test_object::num_copy_ctor_calls() == 0);
    CHECK(rex::test::test_object::num_move_ctor_calls() == 0);
    CHECK(rex::test::test_object::num_copy_assignment_calls() == 1);
    CHECK(rex::test::test_object::num_move_assignment_calls() == 0);
  }

  CHECK(rex::test::test_object::num_created() == 2);
  CHECK(rex::test::test_object::num_ctor_calls() == 2);
  CHECK(rex::test::test_object::num_dtor_calls() == 2);
  CHECK(rex::test::test_object::num_copy_ctor_calls() == 0);
  CHECK(rex::test::test_object::num_move_ctor_calls() == 0);
  CHECK(rex::test::test_object::num_copy_assignment_calls() == 1);
  CHECK(rex::test::test_object::num_move_assignment_calls() == 0);

  {
    rex::test::test_object::reset();

    rex::test::test_object obj{};
    rex::test::test_object obj2{};
    obj2 = rsl::move(obj);

    CHECK(rex::test::test_object::num_created() == 2);
    CHECK(rex::test::test_object::num_ctor_calls() == 2);
    CHECK(rex::test::test_object::num_dtor_calls() == 0);
    CHECK(rex::test::test_object::num_copy_ctor_calls() == 0);
    CHECK(rex::test::test_object::num_move_ctor_calls() == 0);
    CHECK(rex::test::test_object::num_copy_assignment_calls() == 0);
    CHECK(rex::test::test_object::num_move_assignment_calls() == 1);
  }

  CHECK(rex::test::test_object::num_created() == 2);
  CHECK(rex::test::test_object::num_ctor_calls() == 2);
  CHECK(rex::test::test_object::num_dtor_calls() == 2);
  CHECK(rex::test::test_object::num_copy_ctor_calls() == 0);
  CHECK(rex::test::test_object::num_move_ctor_calls() == 0);
  CHECK(rex::test::test_object::num_copy_assignment_calls() == 0);
  CHECK(rex::test::test_object::num_move_assignment_calls() == 1);
}