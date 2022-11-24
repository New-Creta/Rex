// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: iterator.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/internal/iterator/bidirectional_iterator.h"
#include "rex_std/internal/iterator/distance.h"
#include "rex_std/internal/iterator/forward_iterator.h"
#include "rex_std/internal/iterator/iterator_tags.h"
#include "rex_std/internal/iterator/random_access_iterator.h"
#include "rex_std/internal/iterator/reverse_iterator.h"

namespace rsl
{
  inline namespace v1
  {

    /// RSL Comment: Not in ISO C++ Standard at time of writing (17/Aug/2022)
    template <typename It>
    constexpr auto iterator_to_pointer(It it)
    {
      return &(*it);
    }

    /// RSL Comment: Not in ISO C++ Standard at time of writing (17/Aug/2022)
    template <typename It>
    constexpr auto const_iterator_to_pointer(It it)
    {
      return &(*it);
    }

    /// RSL Comment: Not in ISO C++ Standard at time of writing (17/Aug/2022)
    template <typename ReverseIterator>
    constexpr auto to_forward_iterator(ReverseIterator it)
    {
      return (it + 1).base();
    }

  } // namespace v1
} // namespace rsl

#include "rex_std/disable_std_checking.h"
#include "rex_std/std_alias_defines.h"

#include <iterator>

namespace rsl
{
  inline namespace v1
  {
    // No C++20 symbols added here

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename Container>, back_insert_iterator, Container);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename Container>, front_insert_iterator, Container);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename Container>, insert_iterator, Container);

    template <typename CharT>
    class char_traits;

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename T, typename CharT = char8, typename Traits = rsl::char_traits<CharT>, typename Distance = ptrdiff>, istream_iterator, T, CharT, Traits, Distance);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename T, typename CharT = char8, typename Traits = rsl::char_traits<CharT>>, ostream_iterator, T, CharT, Traits);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT, typename Traits = char_traits<CharT>>, ostreambuf_iterator, CharT, Traits);

    REX_STD_FUNC_ALIAS(front_inserter);
    REX_STD_FUNC_ALIAS(back_inserter);
    REX_STD_FUNC_ALIAS(inserter);

    REX_STD_FUNC_ALIAS(operator==);
    REX_STD_FUNC_ALIAS(operator!=);
    REX_STD_FUNC_ALIAS(operator<);
    REX_STD_FUNC_ALIAS(operator<=);
    REX_STD_FUNC_ALIAS(operator>);
    REX_STD_FUNC_ALIAS(operator>=);
    REX_STD_FUNC_ALIAS(operator+);
    REX_STD_FUNC_ALIAS(operator-);

    REX_STD_FUNC_ALIAS(advance);
    REX_STD_FUNC_ALIAS(next);
    REX_STD_FUNC_ALIAS(prev);
    REX_STD_FUNC_ALIAS(cbegin);
    REX_STD_FUNC_ALIAS(cend);
    REX_STD_FUNC_ALIAS(rbegin);
    REX_STD_FUNC_ALIAS(crbegin);
    REX_STD_FUNC_ALIAS(rend);
    REX_STD_FUNC_ALIAS(crend);
    REX_STD_FUNC_ALIAS(size);
    REX_STD_FUNC_ALIAS(ssize);
    REX_STD_FUNC_ALIAS(data);

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"