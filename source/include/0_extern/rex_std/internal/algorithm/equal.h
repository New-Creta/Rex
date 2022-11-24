// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: equal.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename InputIterator1, typename InputIterator2>
    constexpr bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2)
    {
      for(; first1 != last1; ++first1, ++first2)
      {
        if(!(*first1 == *first2)) // Note that we always express value comparisons in terms of < or ==.
        {
          return false;
        }
      }
      return true;
    }

    template <typename InputIterator1, typename InputIterator2, typename BinaryPredicate>
    bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, BinaryPredicate predicate)
    {
      for(; first1 != last1; ++first1, ++first2)
      {
        if(!predicate(*first1, *first2))
        {
          return false;
        }
      }
      return true;
    }

  } // namespace v1
} // namespace rsl
