// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: compressed_pair.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

// A compressed pair uses the empty base class optimization to get rid of one
// of its 2 values from the total size of the pair.
// Consider the following scenario:
//
// struct A
// {
//     // Empty
// };
//
// struct B : A
// {
//     int x;
// };
//
// static_assert(sizeof(A) == 1)
// static_assert(sizeof(B) == 4)
//
// in the example above, struct A has a size of 1 byte because every class or struct in C++ needs to be at least 1 byte big.
// However, for class B, its size is 4, not 5 like you'd expect.
// This is called the empty base class optimization, where if an base class of a class or struct is empty, as in has no member variables,
// nor does it have any virtual functions, then its size is optimized out and the size of the derived class is only the size of the derived class.

// for more info on empty base class optimization, please visit https://en.cppreference.com/w/cpp/language/ebo

#include "rex_std/bonus/type_traits/call_traits.h"
#include "rex_std/bonus/types.h"
#include "rex_std/internal/type_traits/integral_constant.h"
#include "rex_std/internal/type_traits/is_empty.h"
#include "rex_std/internal/type_traits/is_same.h"
#include "rex_std/internal/type_traits/remove_cv.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename T1, typename T2, bool IsSameV, bool FirstEmpty, bool SecondEmpty>
      struct compressed_pair_switch;

      template <typename T1, typename T2>
      struct compressed_pair_switch<T1, T2, false, false, false> : public integral_constant<card32, 0>
      {
      };

      template <typename T1, typename T2>
      struct compressed_pair_switch<T1, T2, false, true, false> : public integral_constant<card32, 1>
      {
      };

      template <typename T1, typename T2>
      struct compressed_pair_switch<T1, T2, false, false, true> : public integral_constant<card32, 2>
      {
      };

      template <typename T1, typename T2>
      struct compressed_pair_switch<T1, T2, false, true, true> : public integral_constant<card32, 3>
      {
      };

      template <typename T1, typename T2>
      struct compressed_pair_switch<T1, T2, true, false, false> : public integral_constant<card32, 4>
      {
      };

      template <typename T1, typename T2>
      struct compressed_pair_switch<T1, T2, true, true, true> : public integral_constant<card32, 5>
      {
      };

      template <typename T1, typename T2, card32 Version>
      struct compressed_pair_impl;

      // Derived from neither
      template <typename T1, typename T2>
      class compressed_pair_impl<T1, T2, 0>
      {
      public:
        using first_type             = T1;
        using second_type            = T2;
        using first_param_type       = typename call_traits<first_type>::param_type;
        using second_param_type      = typename call_traits<second_type>::param_type;
        using first_reference        = typename call_traits<first_type>::reference;
        using second_reference       = typename call_traits<second_type>::reference;
        using first_const_reference  = typename call_traits<first_type>::const_reference;
        using second_const_reference = typename call_traits<second_type>::const_reference;

        compressed_pair_impl() = default;
        compressed_pair_impl(first_param_type first, second_param_type second)
            : m_first(first)
            , m_second(second)
        {
        }
        explicit compressed_pair_impl(first_param_type first)
            : m_first(first)
            , m_second()
        {
        }
        explicit compressed_pair_impl(second_param_type second)
            : m_first()
            , m_second(second)
        {
        }

        first_reference first()
        {
          return m_first;
        }
        first_const_reference first() const
        {
          return m_first;
        }

        second_reference second()
        {
          return m_second;
        }
        second_const_reference second() const
        {
          return m_second;
        }

      private:
        first_type m_first;
        second_type m_second;
      };

      // Derive from T1
      template <typename T1, typename T2>
      class compressed_pair_impl<T1, T2, 1> : private T1
      {
      public:
        using first_type             = T1;
        using second_type            = T2;
        using first_param_type       = typename call_traits<first_type>::param_type;
        using second_param_type      = typename call_traits<second_type>::param_type;
        using first_reference        = typename call_traits<first_type>::reference;
        using second_reference       = typename call_traits<second_type>::reference;
        using first_const_reference  = typename call_traits<first_type>::const_reference;
        using second_const_reference = typename call_traits<second_type>::const_reference;

        compressed_pair_impl() = default;
        compressed_pair_impl(first_param_type first, second_param_type second)
            : first_type(first)
            , m_second(second)
        {
        }
        explicit compressed_pair_impl(first_param_type first)
            : first_type(first)
            , m_second()
        {
        }
        explicit compressed_pair_impl(second_param_type second)
            : first_type()
            , m_second(second)
        {
        }

        first_reference first()
        {
          return *this;
        }
        first_const_reference first() const
        {
          return *this;
        }

        second_reference second()
        {
          return m_second;
        }
        second_const_reference second() const
        {
          return m_second;
        }

      private:
        second_type m_second;
      };

      // Derive from T2
      template <typename T1, typename T2>
      class compressed_pair_impl<T1, T2, 2> : private T2
      {
      public:
        using first_type             = T1;
        using second_type            = T2;
        using first_param_type       = typename call_traits<first_type>::param_type;
        using second_param_type      = typename call_traits<second_type>::param_type;
        using first_reference        = typename call_traits<first_type>::reference;
        using second_reference       = typename call_traits<second_type>::reference;
        using first_const_reference  = typename call_traits<first_type>::const_reference;
        using second_const_reference = typename call_traits<second_type>::const_reference;

        compressed_pair_impl() = default;
        compressed_pair_impl(first_param_type first, second_param_type second)
            : second_type(second)
            , m_first(first)
        {
        }
        explicit compressed_pair_impl(first_param_type first)
            : second_type()
            , m_first(first)
        {
        }
        explicit compressed_pair_impl(second_param_type second)
            : second_type(second)
            , m_first()
        {
        }

        first_reference first()
        {
          return m_first;
        }
        first_const_reference first() const
        {
          return m_first;
        }

        second_reference second()
        {
          return *this;
        }
        second_const_reference second() const
        {
          return *this;
        }

      private:
        first_type m_first;
      };

      // Derive from T1 & T2
      template <typename T1, typename T2>
      class compressed_pair_impl<T1, T2, 3> : private T1, private T2
      {
      public:
        using first_type             = T1;
        using second_type            = T2;
        using first_param_type       = typename call_traits<first_type>::param_type;
        using second_param_type      = typename call_traits<second_type>::param_type;
        using first_reference        = typename call_traits<first_type>::reference;
        using second_reference       = typename call_traits<second_type>::reference;
        using first_const_reference  = typename call_traits<first_type>::const_reference;
        using second_const_reference = typename call_traits<second_type>::const_reference;

        compressed_pair_impl() = default;
        compressed_pair_impl(first_param_type first, second_param_type second)
            : first_type(first)
            , second_type(second)
        {
        }
        explicit compressed_pair_impl(first_param_type first)
            : first_type(first)
            , second_type()
        {
        }
        explicit compressed_pair_impl(second_param_type second)
            : first_type()
            , second_type(second)
        {
        }

        first_reference first()
        {
          return *this;
        }
        first_const_reference first() const
        {
          return *this;
        }

        second_reference second()
        {
          return *this;
        }
        second_const_reference second() const
        {
          return *this;
        }
      };

      // T1 == T2 and are not empty
      template <typename T1, typename T2>
      class compressed_pair_impl<T1, T2, 4>
      {
      public:
        using first_type             = T1;
        using second_type            = T2;
        using first_param_type       = typename call_traits<first_type>::param_type;
        using second_param_type      = typename call_traits<second_type>::param_type;
        using first_reference        = typename call_traits<first_type>::reference;
        using second_reference       = typename call_traits<second_type>::reference;
        using first_const_reference  = typename call_traits<first_type>::const_reference;
        using second_const_reference = typename call_traits<second_type>::const_reference;

        compressed_pair_impl() = default;
        compressed_pair_impl(first_param_type first, second_param_type second)
            : m_first(first)
            , m_second(second)
        {
        }
        explicit compressed_pair_impl(first_param_type first)
            : m_first(first)
            , m_second()
        {
        }
        explicit compressed_pair_impl(second_param_type second)
            : m_first()
            , m_second(second)
        {
        }

        first_reference first()
        {
          return m_first;
        }
        first_const_reference first() const
        {
          return m_first;
        }

        second_reference second()
        {
          return m_second;
        }
        second_const_reference second() const
        {
          return m_second;
        }

      private:
        first_type m_first;
        second_type m_second;
      };

      // T1 == T2, T1 and T2 are both empty
      // Note does not actually store an instance of T2 at all;
      // but reuses T1 base class for both first() and second().
      template <typename T1, typename T2>
      class compressed_pair_impl<T1, T2, 5> : private T1
      {
      public:
        using first_type             = T1;
        using second_type            = T2;
        using first_param_type       = typename call_traits<first_type>::param_type;
        using second_param_type      = typename call_traits<second_type>::param_type;
        using first_reference        = typename call_traits<first_type>::reference;
        using second_reference       = typename call_traits<second_type>::reference;
        using first_const_reference  = typename call_traits<first_type>::const_reference;
        using second_const_reference = typename call_traits<second_type>::const_reference;

        compressed_pair_impl() = default;
        compressed_pair_impl(first_param_type first, second_param_type second)
            : first_type(first)
        {
        }
        explicit compressed_pair_impl(first_param_type first)
            : first_type(first)
        {
        }
        explicit compressed_pair_impl(second_param_type second)
            : first_type()
        {
        }

        first_reference first()
        {
          return *this;
        }
        first_const_reference first() const
        {
          return *this;
        }

        second_reference second()
        {
          return *this;
        }
        second_const_reference second() const
        {
          return *this;
        }
      };

    } // namespace internal

    template <typename T1, typename T2>
    struct compressed_pair : private internal::compressed_pair_impl<T1, T2, internal::compressed_pair_switch<T1, T2, is_same_v<remove_cv_t<T1>, remove_cv_t<T2>>, is_empty_v<T1>, is_empty_v<T2>>::value>
    {
    private:
      using base = internal::compressed_pair_impl<T1, T2, internal::compressed_pair_switch<T1, T2, is_same_v<remove_cv_t<T1>, remove_cv_t<T2>>, is_empty_v<T1>, is_empty_v<T2>>::value>;

    public:
      using first_type             = T1;
      using second_type            = T2;
      using first_param_type       = typename call_traits<first_type>::param_type;
      using second_param_type      = typename call_traits<second_type>::param_type;
      using first_reference        = typename call_traits<first_type>::reference;
      using second_reference       = typename call_traits<second_type>::reference;
      using first_const_reference  = typename call_traits<first_type>::const_reference;
      using second_const_reference = typename call_traits<second_type>::const_reference;

      compressed_pair() = default;
      compressed_pair(first_param_type first, second_param_type second)
          : base(first, second)
      {
      }
      explicit compressed_pair(first_param_type first)
          : base(first)
      {
      }
      explicit compressed_pair(second_param_type second)
          : base(second)
      {
      }

      first_reference first()
      {
        return base::first();
      }
      first_const_reference first() const
      {
        return base::first();
      }

      second_reference second()
      {
        return base::second();
      }
      second_const_reference second() const
      {
        return base::second();
      }
    };

    template <typename T>
    struct compressed_pair<T, T> : private internal::compressed_pair_impl<T, T, internal::compressed_pair_switch<T, T, is_same_v<remove_cv_t<T>, remove_cv_t<T>>, is_empty_v<T>, is_empty_v<T>>::value>
    {
    private:
      using base = internal::compressed_pair_impl<T, T, internal::compressed_pair_switch<T, T, is_same_v<remove_cv_t<T>, remove_cv_t<T>>, is_empty_v<T>, is_empty_v<T>>::value>;

    public:
      using first_type             = T;
      using second_type            = T;
      using first_param_type       = typename call_traits<first_type>::param_type;
      using second_param_type      = typename call_traits<second_type>::param_type;
      using first_reference        = typename call_traits<first_type>::reference;
      using second_reference       = typename call_traits<second_type>::reference;
      using first_const_reference  = typename call_traits<first_type>::const_reference;
      using second_const_reference = typename call_traits<second_type>::const_reference;

      compressed_pair() = default;
      compressed_pair(first_param_type first, second_param_type second)
          : base(first, second)
      {
      }
      explicit compressed_pair(first_param_type first)
          : base(first)
      {
      }

      first_reference first()
      {
        return base::first();
      }
      first_const_reference first() const
      {
        return base::first();
      }

      second_reference second()
      {
        return base::second();
      }
      second_const_reference second() const
      {
        return base::second();
      }
    };

  } // namespace v1
} // namespace rsl
