// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: pair.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/defines.h"
#include "rex_std/bonus/type_traits/remove_reference_wrapper.h"
#include "rex_std/bonus/utility/use_first.h"
#include "rex_std/bonus/utility/use_second.h"
#include "rex_std/internal/type_traits/decay.h"
#include "rex_std/internal/type_traits/enable_if.h"
#include "rex_std/internal/type_traits/is_array.h"
#include "rex_std/internal/type_traits/is_convertible.h"
#include "rex_std/internal/type_traits/is_copy_constructible.h"
#include "rex_std/internal/type_traits/is_default_constructible.h"
#include "rex_std/internal/utility/forward.h"
#include "rex_std/internal/utility/move.h"
#include "rex_std/internal/utility/piecewise_construct.h"
#include "rex_std/tuple.h"

namespace rsl
{
  inline namespace v1
  {

    STATIC_WARNING("using rsl::pair - this is deprecated, please use a named struct");

    template <typename T1, typename T2>
    struct pair
    {
      static_assert(!is_array_v<T1>, "T1 can't be of array type");
      static_assert(!is_array_v<T2>, "T2 can't be of array type");

      using first_type  = T1;
      using second_type = T2;
      using this_type   = pair<T1, T2>;

      T1 first;
      T2 second;

      template <typename TT1 = T1, typename TT2 = T2, typename = enable_if_t<is_default_constructible_v<TT1> && is_default_constructible_v<TT2>>>
      constexpr pair()
          : first()
          , second()
      {
      }

      template <typename TT1 = T1, typename TT2 = T2, typename = enable_if_t<is_copy_constructible_v<TT1> && is_copy_constructible_v<TT2>>>
      constexpr pair(const T1& x, const T2& y)
          : first(x)
          , second(y)
      {
      }

      constexpr pair(pair&& p)    = default;
      constexpr pair(const pair&) = default;

      template <typename U, typename V, typename = enable_if_t<is_convertible_v<const U&, T1> && is_convertible_v<const V&, T2>>>
      constexpr explicit pair(const pair<U, V>& p)
          : first(p.first)
          , second(p.second)
      {
      }

      template <typename U, typename V, typename = enable_if_t<is_convertible_v<U, T1> && is_convertible_v<V, T2>>>
      constexpr pair(U&& u, V&& v)
          : first(forward<U>(u))
          , second(forward<V>(v))
      {
      }

      template <typename U, typename = enable_if_t<is_convertible_v<U, T1>>>
      constexpr pair(U&& x, const T2& y)
          : first(forward<U>(x))
          , second(y)
      {
      }

      template <typename V, typename = enable_if_t<is_convertible_v<V, T2>>>
      constexpr pair(const T1& x, V&& y)
          : first(x)
          , second(forward<V>(y))
      {
      }

      template <typename U, typename V, typename = enable_if_t<is_convertible_v<U, T1> && is_convertible_v<V, T2>>>
      constexpr explicit pair(pair<U, V>&& p)
          : first(forward<U>(p.first))
          , second(forward<V>(p.second))
      {
      }

      // Initializes first with arguments of types Args1... obtained by forwarding the elements of first_args and
      // initializes second with arguments of types Args2... obtained by forwarding the elements of second_args.
      template <typename... Args1, typename... Args2, typename = enable_if_t<is_constructible_v<first_type, Args1&&...> && is_constructible_v<second_type, Args2&&...>>>
      pair(piecewise_construct_t pwc, tuple<Args1...> firstArgs, tuple<Args2...> secondArgs)
          : pair(pwc, move(firstArgs), move(secondArgs), make_index_sequence<sizeof...(Args1)>(), make_index_sequence<sizeof...(Args2)>())
      {
      }

      ~pair() = default;

    private:
      // NOTE(rparolin): Internal constructor used to expand the index_sequence required to expand the tuple elements.
      template <typename... Args1, typename... Args2, count_t... I1, count_t... I2>
      pair(piecewise_construct_t /*unused*/, tuple<Args1...> firstArgs, tuple<Args2...> secondArgs, index_sequence<I1...> /*unused*/, index_sequence<I2...> /*unused*/)
          : first(forward<Args1>(get<I1>(firstArgs))...)
          , second(forward<Args2>(get<I2>(secondArgs))...)
      {
      }

    public:
      pair& operator=(const pair& p)
      {
        first  = p.first;
        second = p.second;
        return *this;
      }

      template <typename U, typename V, typename = enable_if_t<is_convertible_v<U, T1> && is_convertible_v<V, T2>>>
      pair& operator=(const pair<U, V>& p)
      {
        first  = p.first;
        second = p.second;
        return *this;
      }

      pair& operator=(pair&& p)
      {
        first  = forward<T1>(p.first);
        second = forward<T2>(p.second);
        return *this;
      }

      template <typename U, typename V, typename = enable_if_t<is_convertible_v<U, T1> && is_convertible_v<V, T2>>>
      pair& operator=(pair<U, V>&& p)
      {
        first  = forward<U>(p.first);
        second = forward<V>(p.second);
        return *this;
      }

      void swap(pair& p)
      {
        iter_swap(&first, &p.first);
        iter_swap(&second, &p.second);
      }
    };

    template <typename T1, typename T2>
    constexpr pair<T1, T2> make_pair(const T1& a, const T2& b)
    {
      return pair<T1, T2>(a, b);
    }

    template <typename T1, typename T2>
    constexpr pair<typename remove_reference_wrapper<decay_t<T1>>::type, typename remove_reference_wrapper<decay_t<T2>>::type> make_pair(T1&& a, T2&& b)
    {
      using T1Type = typename remove_reference_wrapper<decay_t<T1>>::type;
      using T2Type = typename remove_reference_wrapper<decay_t<T2>>::type;

      return pair<T1Type, T2Type>(rsl::forward<T1>(a), rsl::forward<T2>(b));
    }

    template <typename T1, typename T2>
    constexpr bool operator==(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
    {
      return lhs.first == rhs.first && lhs.second == rhs.second;
    }
    template <typename T1, typename T2>
    constexpr bool operator!=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
    {
      return !(lhs == rhs);
    }
    template <typename T1, typename T2>
    constexpr bool operator<(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
    {
      // we could use the simpler: return (a.m1 == b.m1) ? (a.m2 <b.m2) : (a.m1 <b.m1);
      // if the user wants more performance, they can write a specialization for this.
      return ((lhs.first < rhs.first) || (!(rhs.first < lhs.first) && (lhs.second < rhs.second)));
    }
    template <typename T1, typename T2>
    constexpr bool operator>(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
    {
      return rhs < lhs;
    }
    template <typename T1, typename T2>
    constexpr bool operator>=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
    {
      return !(lhs < rhs);
    }
    template <typename T1, typename T2>
    constexpr bool operator<=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
    {
      return !(rhs < lhs);
    }

    template <typename T1, typename T2>
    constexpr void swap(pair<T1, T2>& x, pair<T1, T2>& y)
    {
      swap(x.first, y.first);
      swap(x.second, y.second);
    }

    /// RSL Comment: Different from ISO C++ Standard at time of writing (22/Jul/2022)
    // the standard also defines a swap for const pair.
    // this is not supported in rsl

    namespace internal
    {
      template <typename Ret, typename Pair>
      constexpr Ret pair_get(Pair& p, integral_constant<count_t, 0> /*unused*/)
      {
        return p.first;
      }
      template <typename Ret, typename Pair>
      constexpr Ret pair_get(Pair& p, integral_constant<count_t, 1> /*unused*/)
      {
        return p.second;
      }
    } // namespace internal

    template <count_t I, typename T1, typename T2>
    constexpr tuple_element_t<I, pair<T1, T2>>& get(pair<T1, T2>& p)
    {
      static_assert(I == 0 || I == 1, "I is out of range");
      using ReturnType = tuple_element_t<I, pair<T1, T2>>&;
      return internal::pair_get<ReturnType>(p, integral_constant<count_t, I> {});
    }

    template <count_t I, typename T1, typename T2>
    constexpr const tuple_element_t<I, pair<T1, T2>>& get(const pair<T1, T2>& p)
    {
      static_assert(I == 0 || I == 1, "I is out of range");
      using ReturnType = const tuple_element_t<I, pair<T1, T2>>&;
      return internal::pair_get<ReturnType>(p, integral_constant<count_t, I> {});
    }

    template <count_t I, typename T1, typename T2>
    constexpr tuple_element_t<I, pair<T1, T2>>&& get(pair<T1, T2>&& p)
    {
      static_assert(I == 0 || I == 1, "I is out of range");
      using ReturnType = tuple_element_t<I, pair<T1, T2>>&&;
      return rsl::forward<ReturnType>(get<I>(p));
    }

    template <count_t I, typename T1, typename T2>
    constexpr const tuple_element_t<I, pair<T1, T2>>&& get(const pair<T1, T2>&& p)
    {
      static_assert(I == 0 || I == 1, "I is out of range");
      using ReturnType = const tuple_element_t<I, pair<T1, T2>>&&;
      return rsl::forward<ReturnType>(get<I>(p));
    }

    template <typename T, typename U>
    constexpr T& get(pair<T, U>& p)
    {
      return p.first;
    }

    template <typename T, typename U>
    constexpr const T& get(const pair<T, U>& p)
    {
      return p.first;
    }

    template <typename T, typename U>
    constexpr T&& get(pair<T, U>&& p)
    {
      return rsl::move(p.first);
    }

    template <typename T, typename U>
    constexpr const T&& get(const pair<T, U>&& p)
    {
      return rsl::move(p.first);
    }

    template <typename T, typename U>
    constexpr T& get(pair<U, T>& p)
    {
      return p.second;
    }

    template <typename T, typename U>
    constexpr const T& get(const pair<U, T>& p)
    {
      return p.second;
    }

    template <typename T, typename U>
    constexpr T&& get(pair<U, T>&& p)
    {
      return rsl::move(p.second);
    }

    template <typename T, typename U>
    constexpr const T&& get(const pair<U, T>&& p)
    {
      return rsl::move(p.second);
    }

    template <typename T1, typename T2>
    struct tuple_size<pair<T1, T2>> : integral_constant<count_t, 2>
    {
    };

    template <count_t I, typename T1, typename T2>
    struct tuple_element<I, pair<T1, T2>>;

    template <count_t I, typename T1, typename T2>
    struct tuple_element<I, pair<T1, T2>>
    {
      static_assert(I < 2, "pair has only 2 elements!");
    };

    template <typename T1, typename T2>
    struct tuple_element<0, pair<T1, T2>>
    {
      using type = T1;
    };

    template <typename T1, typename T2>
    struct tuple_element<1, pair<T1, T2>>
    {
      using type = T2;
    };

    template <count_t I, typename T>
    using tuple_element_t = typename tuple_element<I, T>::type;

    template <typename T1, typename T2>
    pair(T1, T2) -> pair<T1, T2>;

    /// RSL Comment: Not in ISO C++ Standard at time of writing (30/Aug/2022)
    struct pair_first_construct_t
    {
    };
    constexpr pair_first_construct_t pair_first_construct = pair_first_construct_t();

  } // namespace v1
} // namespace rsl
