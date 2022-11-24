// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: mismatch.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename Iterator1, typename Iterator2>
    struct mismatch_result
    {
      Iterator1 lhs_it;
      Iterator2 rhs_it;
    };

    template <typename InputIterator1, typename InputIterator2>
    mismatch_result<InputIterator1, InputIterator2> mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2)
    {
      auto lhs_it = first1;
      auto rhs_it = first2;
      while(lhs_it != last1 && *lhs_it == *rhs_it)
      {
        ++lhs_it, ++rhs_it;
      }
      return mismatch_result<InputIterator1, InputIterator2> {lhs_it, rhs_it};
    }

    template <typename InputIterator1, typename InputIterator2, typename Predicate>
    mismatch_result<InputIterator1, InputIterator2> mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, Predicate predicate)
    {
      auto lhs_it = first1;
      auto rhs_it = first2;
      while(lhs_it != last1 && predicate(*lhs_it, *rhs_it))
      {
        ++lhs_it, ++rhs_it;
      }
      return mismatch_result<InputIterator1, InputIterator2> {lhs_it, rhs_it};
    }

    template <typename InputIterator1, typename InputIterator2>
    mismatch_result<InputIterator1, InputIterator2> mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2)
    {
      auto lhs_it = first1;
      auto rhs_it = first2;
      while(lhs_it != last1 && rhs_it != last2 && *lhs_it == *rhs_it)
      {
        ++lhs_it, ++rhs_it;
      }
      return mismatch_result<InputIterator1, InputIterator2> {lhs_it, rhs_it};
    }

    template <typename InputIterator1, typename InputIterator2, typename Predicate>
    mismatch_result<InputIterator1, InputIterator2> mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, Predicate predicate)
    {
      auto lhs_it = first1;
      auto rhs_it = first2;
      while(lhs_it != last1 && rhs_it != last2 && predicate(*lhs_it, *rhs_it))
      {
        ++lhs_it, ++rhs_it;
      }
      return mismatch_result<InputIterator1, InputIterator2> {lhs_it, rhs_it};
    }

  } // namespace v1
} // namespace rsl
