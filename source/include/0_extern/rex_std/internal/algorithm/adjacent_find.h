// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: adjacent_find.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename ForwardIterator>
    ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last)
    {
      if(first != last)
      {
        ForwardIterator i = first;

        for(++i; i != last; ++i)
        {
          if(*first == *i)
            return first;
          first = i;
        }
      }
      return last;
    }

    template <typename ForwardIterator, typename BinaryPredicate>
    ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last, BinaryPredicate predicate)
    {
      if(first != last)
      {
        ForwardIterator i = first;

        for(++i; i != last; ++i)
        {
          if(predicate(*first, *i))
            return first;
          first = i;
        }
      }
      return last;
    }

  } // namespace v1
} // namespace rsl
