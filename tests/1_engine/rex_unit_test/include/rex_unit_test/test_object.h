#pragma once

#include "rex_std/bonus/types.h"

namespace rex::test
{
  class test_object
  {
  public:
    test_object(card32 x = 0);
    test_object(card32 x, card32 y, card32 z);
    test_object(const test_object& obj);
    test_object(test_object&& obj);
    ~test_object();

    test_object& operator=(const test_object& obj);
    test_object& operator=(test_object&& obj);

    card32 x() const;
    operator card32() const; // this is to easily compare with integers

    static bool is_clear();
    static void reset();
    static card32 num_created();
    static card32 num_ctor_calls();
    static card32 num_dtor_calls();
    static card32 num_copy_ctor_calls();
    static card32 num_move_ctor_calls();
    static card32 num_copy_assignment_calls();
    static card32 num_move_assignment_calls();

  private:
    card32 m_x; // value for this instance
    static card32 s_num_created; // number of created objects
    static card32 s_num_ctor_calls; // number of constructor calls
    static card32 s_num_dtor_calls; // number of constructor calls
    static card32 s_num_copy_ctor_calls; // number of copy constructor calls
    static card32 s_num_move_ctor_calls; // number of move constructor calls
    static card32 s_num_copy_assignment_calls; // number of copy assignment calls
    static card32 s_num_move_assignment_calls; // number of move assignment calls
  };

  bool operator==(const test_object& lhs, const test_object& rhs);
  bool operator==(const test_object& lhs, card32 rhs);
  bool operator==(card32 lhs, const test_object& rhs);
  bool operator!=(const test_object& lhs, const test_object& rhs);
  bool operator!=(const test_object& lhs, card32 rhs);
  bool operator!=(card32 lhs, const test_object& rhs);
}
