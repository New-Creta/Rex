#include "rex_test/test_object.h"

namespace rex::test
{
  inline namespace v1
  {
    card32 test_object::s_num_created = 0; // number of current alive test objects
    card32 test_object::s_num_ctor_calls = 0; // number of constructor calls
    card32 test_object::s_num_dtor_calls = 0; // number of constructor calls
    card32 test_object::s_num_copy_ctor_calls = 0; // number of copy constructor calls
    card32 test_object::s_num_move_ctor_calls = 0; // number of move constructor calls
    card32 test_object::s_num_copy_assignment_calls = 0; // number of copy assignment calls
    card32 test_object::s_num_move_assignment_calls = 0; // number of move assignment calls

    test_object::test_object(card32 x)
      : m_x(x)
    {
      s_num_created++;
      s_num_ctor_calls++;
    }
    test_object::test_object(card32 x, card32 y, card32 z)
      : m_x(x + y + z)
    {
      s_num_created++;
      s_num_ctor_calls++;
    }

    test_object::test_object(const test_object& obj)
      : m_x(obj.m_x)
    {
      s_num_created++;
      s_num_copy_ctor_calls++;
    }
    test_object::test_object(test_object&& obj)
      : m_x(obj.m_x)
    {
      s_num_created++;
      s_num_move_ctor_calls++;
    }
    test_object::~test_object()
    {
      s_num_dtor_calls++;
    }

    test_object& test_object::operator=(const test_object& obj)
    {
      m_x = obj.m_x;
      s_num_copy_assignment_calls++;

      return *this;
    }
    test_object& test_object::operator=(test_object&& obj)
    {
      m_x = obj.m_x;
      s_num_move_assignment_calls++;

      return *this;
    }

    card32 test_object::x() const
    {
      return m_x;
    }

    test_object::operator rsl::v1::card32() const
    {
      return m_x;
    }
    
    bool test_object::is_clear()
    {
      return (s_num_created == s_num_dtor_calls);
    }

    void test_object::reset()
    {
      s_num_created = 0;

      s_num_ctor_calls = 0;
      s_num_dtor_calls = 0;
      s_num_copy_ctor_calls = 0;
      s_num_move_ctor_calls = 0;
      s_num_copy_assignment_calls = 0;
      s_num_move_assignment_calls = 0;
    }

    card32 test_object::num_created()
    {
      return s_num_created;
    }
    card32 test_object::num_ctor_calls()
    {
      return s_num_ctor_calls;
    }
    card32 test_object::num_dtor_calls()
    {
      return s_num_dtor_calls;
    }
    card32 test_object::num_copy_ctor_calls()
    {
      return s_num_copy_ctor_calls;
    }
    card32 test_object::num_move_ctor_calls()
    {
      return s_num_move_ctor_calls;
    }
    card32 test_object::num_copy_assignment_calls()
    {
      return s_num_copy_assignment_calls;
    }
    card32 test_object::num_move_assignment_calls()
    {
      return s_num_move_assignment_calls;
    }

    bool operator==(const test_object& lhs, const test_object& rhs)
    {
      return lhs.x() == rhs.x();
    }
    bool operator==(const test_object& lhs, card32 rhs)
    {
      return lhs.x() == rhs;
    }
    bool operator==(card32 lhs, const test_object& rhs)
    {
      return lhs == rhs.x();
    }
    bool operator!=(const test_object& lhs, const test_object& rhs)
    {
      return !(lhs == rhs);
    }
    bool operator!=(const test_object& lhs, card32 rhs)
    {
      return !(lhs == rhs);
    }
    bool operator!=(card32 lhs, const test_object& rhs)
    {
      return !(lhs == rhs);
    }
  }
}