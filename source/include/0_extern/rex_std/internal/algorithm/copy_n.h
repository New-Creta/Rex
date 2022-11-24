// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: copy_n.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/algorithm/copy.h"
#include "rex_std/internal/iterator/iterator_tags.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename InputIterator, typename Size, typename OutputIterator>
      OutputIterator copy_n_impl(InputIterator first, Size n, OutputIterator result, rsl::input_iterator_tag /*unused*/)
      {
        for(; n > 0; --n)
          *result++ = *first++;
        return result;
      }

      template <typename RandomAccessIterator, typename Size, typename OutputIterator>
      OutputIterator copy_n_impl(RandomAccessIterator first, Size n, OutputIterator result, rsl::random_access_iterator_tag /*unused*/)
      {
        return rsl::copy(first, first + n, result); // Take advantage of the optimizations present in the copy algorithm.
      }
    } // namespace internal

    template <typename InputIterator, typename Size, typename OutputIterator>
    OutputIterator copy_n(InputIterator first, Size n, OutputIterator dstFirst)
    {
      using IC = typename InputIterator::iterator_category;
      return internal::copy_n_impl(first, n, dstFirst, IC());
    }

  } // namespace v1
} // namespace rsl
