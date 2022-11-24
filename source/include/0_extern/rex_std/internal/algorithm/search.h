// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: search.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <class ForwardIterator1, class ForwardIterator2>
    constexpr ForwardIterator1 search(ForwardIterator1 first, ForwardIterator1 last, ForwardIterator2 s_first, ForwardIterator2 s_last)
    {
      while(true)
      {
        ForwardIterator1 it = first;
        for(ForwardIterator2 s_it = s_first;; ++it, ++s_it)
        {
          if(s_it == s_last)
            return first;
          if(it == last)
            return last;

          if(!(*it == *s_it))
            break;
        }
        ++first;
      }
    }

    template <class ForwardIterator1, class ForwardIterator2, class Predicate>
    constexpr ForwardIterator1 search(ForwardIterator1 first, ForwardIterator1 last, ForwardIterator2 s_first, ForwardIterator2 s_last, Predicate p)
    {
      while(true)
      {
        ForwardIterator1 it = first;
        for(ForwardIterator2 s_it = s_first;; ++it, ++s_it)
        {
          if(s_it == s_last)
            return first;
          if(it == last)
            return last;
          if(!p(*it, *s_it))
            break;
        }
        ++first;
      }
    }

  } // namespace v1
} // namespace rsl
