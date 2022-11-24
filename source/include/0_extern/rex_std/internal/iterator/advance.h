// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: advance.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/iterator/iterator_tags.h"
#include "rex_std/internal/iterator/iterator_traits.h"
#include "rex_std/internal/type_traits/is_base_of.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename Iterator, typename Distance>
      void random_access_advance(Iterator& it, Distance distance)
      {
        it += distance;
      }

      template <typename Iterator, typename Distance>
      void bidirectional_advance(Iterator& it, Distance distance)
      {
        while(distance > 0)
        {
          ++it;
        }
        while(distance < 0)
        {
          --it;
        }
      }

      template <typename Iterator, typename Distance>
      void forward_advance(Iterator& it, Distance distance)
      {
        while(distance > 0)
        {
          ++it;
        }
      }
    } // namespace internal

    template <typename Iterator, typename Distance>
    void advance(Iterator& it, Distance distance)
    {
      // Random access
      if constexpr(is_base_of_v<random_access_iterator_tag, typename iterator_traits<Iterator>::iterator_category>)
      {
        internal::random_access_advance(it, distance);
      }
      else if constexpr(is_base_of_v<bidirectional_iterator_tag, typename iterator_traits<Iterator>::iterator_category>)
      {
        internal::bidirectional_advance(it, distance);
      }
      else
      {
        internal::forward_advance(it, distance);
      }
    }

  } // namespace v1
} // namespace rsl
