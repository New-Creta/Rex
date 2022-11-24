// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: lexicographical_compare.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/internal/algorithm/memcmp.h"
#include "rex_std/internal/algorithm/min.h"

namespace rsl
{
  inline namespace v1
  {
    template <typename InputIterator1, typename InputIterator2>
    constexpr bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2)
    {
      for(; (first1 != last1) && (first2 != last2); ++first1, ++first2)
      {
        if(*first1 < *first2)
        {
          return true;
        }
        if(*first2 < *first1)
        {
          return false;
        }
      }
      return (first1 == last1) && (first2 != last2);
    }

    // Specialization for const char8*.
    constexpr bool lexicographical_compare(const char8* first1, const char8* last1, const char8* first2, const char8* last2)
    {
      const ptrdiff n1(last1 - first1);
      const ptrdiff n2(last2 - first2);
      const int result = rsl::memcmp(first1, first2, static_cast<count_t>((rsl::min)(n1, n2)));
      return result ? (result < 0) : (n1 < n2); // NOLINT(readability-implicit-bool-conversion)
    }

    // Specialization for char8*.
    constexpr bool lexicographical_compare(char8* first1, char8* last1, char8* first2, char8* last2) // NOLINT(readability-non-const-parameter)
    {
      const ptrdiff n1(last1 - first1);
      const ptrdiff n2(last2 - first2);
      const int result = rsl::memcmp(first1, first2, static_cast<count_t>((rsl::min)(n1, n2)));
      return result ? (result < 0) : (n1 < n2); // NOLINT(readability-implicit-bool-conversion)
    }

    // Specialization for const uint8*.
    constexpr bool lexicographical_compare(const uint8* first1, const uint8* last1, const uint8* first2, const uint8* last2)
    {
      const ptrdiff n1(last1 - first1);
      const ptrdiff n2(last2 - first2);
      const int result = rsl::memcmp(first1, first2, static_cast<count_t>((rsl::min)(n1, n2)));
      return result ? (result < 0) : (n1 < n2); // NOLINT(readability-implicit-bool-conversion)
    }

    // Specialization for uint8*.
    constexpr bool lexicographical_compare(uint8* first1, uint8* last1, uint8* first2, uint8* last2) // NOLINT(readability-non-const-parameter)
    {
      const ptrdiff n1(last1 - first1);
      const ptrdiff n2(last2 - first2);
      const int result = rsl::memcmp(first1, first2, static_cast<count_t>((rsl::min)(n1, n2)));
      return result ? (result < 0) : (n1 < n2); // NOLINT(readability-implicit-bool-conversion)
    }

    // Specialization for const int8*.
    constexpr bool lexicographical_compare(const int8* first1, const int8* last1, const int8* first2, const int8* last2)
    {
      const ptrdiff n1(last1 - first1);
      const ptrdiff n2(last2 - first2);
      const int result = rsl::memcmp(first1, first2, static_cast<count_t>((rsl::min)(n1, n2)));
      return result ? (result < 0) : (n1 < n2); // NOLINT(readability-implicit-bool-conversion)
    }

    // Specialization for int8*.
    constexpr bool lexicographical_compare(int8* first1, int8* last1, int8* first2, int8* last2) // NOLINT(readability-non-const-parameter)
    {
      const ptrdiff n1(last1 - first1);
      const ptrdiff n2(last2 - first2);
      const int result = rsl::memcmp(first1, first2, static_cast<count_t>((rsl::min)(n1, n2)));
      return result ? (result < 0) : (n1 < n2); // NOLINT(readability-implicit-bool-conversion)
    }
  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"