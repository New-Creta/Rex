// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: search_n.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <class ForwardIterator, class Size, class T>
    ForwardIterator search_n(ForwardIterator first, ForwardIterator last, Size count, const T& value)
    {
      if(count <= 0)
      {
        return first;
      }
      for(; first != last; ++first)
      {
        if(!(*first == value))
        {
          continue;
        }

        ForwardIterator candidate = first;
        Size cur_count            = 0;

        while(true)
        {
          ++cur_count;
          if(cur_count >= count)
          {
            // success
            return candidate;
          }
          ++first;
          if(first == last)
          {
            // exhausted the list
            return last;
          }
          if(*first != value)
          {
            // too few in a row
            break;
          }
        }
      }
      return last;
    }

    template <class ForwardIterator, class Size, class T, class Predicate>
    ForwardIterator search_n(ForwardIterator first, ForwardIterator last, Size count, const T& value, Predicate p)
    {
      if(count <= 0)
      {
        return first;
      }
      for(; first != last; ++first)
      {
        if(!p(*first, value))
        {
          continue;
        }

        ForwardIterator candidate = first;
        Size cur_count            = 0;

        while(true)
        {
          ++cur_count;
          if(cur_count >= count)
          {
            // success
            return candidate;
          }
          ++first;
          if(first == last)
          {
            // exhausted the list
            return last;
          }
          if(!p(*first, value))
          {
            // too few in a row
            break;
          }
        }
      }
      return last;
    }

  } // namespace v1
} // namespace rsl
