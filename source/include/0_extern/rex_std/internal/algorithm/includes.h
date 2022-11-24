// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: includes.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename InputIt1, typename InputIt2>
    bool includes(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2)
    {
      for(; first2 != last2; ++first1)
      {
        if(first1 == last1 || *first2 < *first1)
          return false;
        if(!(*first1 < *first2))
          ++first2;
      }
      return true;
    }

    template <typename InputIt1, typename InputIt2, typename Compare>
    bool includes(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, Compare comp)
    {
      for(; first2 != last2; ++first1)
      {
        if(first1 == last1 || comp(*first2, *first1))
          return false;
        if(!comp(*first1, *first2))
          ++first2;
      }
      return true;
    }

  } // namespace v1
} // namespace rsl
