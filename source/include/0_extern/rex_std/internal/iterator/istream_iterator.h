// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: istream_iterator.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// https://en.cppreference.com/w/cpp/iterator/istream_iterator
//
// single-pass input iterator that reads successive objects of type T from the basic_istream.
//-----------------------------------------------------------------------------

#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/internal/iterator/iterator_tags.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T, typename CharT = char8, typename Traits = char_traits<CharT>, typename Distance = ptrdiff>
    class istream_iterator
    {
      using iterator_category = input_iterator_tag;
      using value_type        = T;
      using difference_type   = Distance;
      using pointer           = const T*;
      using reference         = const T&;
      using char_type         = CharT;
      using traits_type       = Traits;
      using istream_type      = basic_istream<CharT, Traits>;

      constexpr istream_iterator();
      istream_iterator(stream_type& stream);
      istream_iterator(const istream_iterator& stream) = default;

      ~istream_iterator() = default;

      const T& operator*() const;
      const T* operator->() const;

      istream_iterator& operator++();
      istream_iterator operator++(int);
    };

    template <typename T, typename CharT, typename Traits, typename Dist>
    bool operator==(const istream_iterator<T, CharT, Traits, Dist>& lhs, const istream_iterator<T, CharT, Traits, Dist>& rhs);

    template <typename T, typename CharT, typename Traits, typename Dist>
    bool operator!=(const istream_iterator<T, CharT, Traits, Dist>& lhs, const istream_iterator<T, CharT, Traits, Dist>& rhs);

  } // namespace v1
} // namespace rsl