// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: minmax_element.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/functional/less.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename Iterator>
    struct min_max_element_result
    {
      Iterator min;
      Iterator max;
    };

    template <typename InputIterator>
    min_max_element_result<InputIterator> minmax_element(InputIterator first, InputIterator last)
    {
      using value_type = InputIterator::value_type;
      return minmax_element(first, last, rsl::less<value_type>());
    }

    template <typename InputIterator, typename Compare>
    min_max_element_result<InputIterator> minmax_element(InputIterator first, InputIterator last, Compare compare)
    {
      auto min = first, max = first;

      if(first == last || ++first == last)
        return {min, max};

      if(compare(*first, *min))
      {
        min = first;
      }
      else
      {
        max = first;
      }

      while(++first != last)
      {
        auto it = first;
        if(++first == last)
        {
          if(compare(*it, *min))
          {
            min = it;
          }
          else if(!(compare(*it, *max)))
          {
            max = it;
          }
          break;
        }
        else
        {
          if(compare(*first, *it))
          {
            if(compare(*first, *min))
            {
              min = first;
            }
            if(!(compare(*it, *max)))
            {
              max = it;
            }
          }
          else
          {
            if(compare(*it, *min))
            {
              min = it;
            }
            if(!(compare(*first, *max)))
            {
              max = first;
            }
          }
        }
      }
      return {min, max};
    }

  } // namespace v1
} // namespace rsl
