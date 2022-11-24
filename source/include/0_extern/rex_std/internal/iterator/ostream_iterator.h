// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: ostream_iterator.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// https://en.cppreference.com/w/cpp/iterator/ostream_iterator
//
// Single-pass that writes successive objects of type T into the basic_ostream
//-----------------------------------------------------------------------------

#pragma once

#include "rex_std/bonus/types.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T, typename CharT = char8, typename Traits = char_traits<CharT>>
    class ostream_iterator
    {
    public:
      using iterator_category = output_iterator_tag;
      using value_type        = void;
      using difference_type   = ptrdiff;
      using pointer           = void;
      using reference         = void;
      using char_type         = CharT;
      using traits_type       = Traits;
      using ostream_type      = basic_ostream<CharT, Traits>;

      ostream_iterator(ostream_type& stream, const CharT* delim);
      ostream_iterator(ostream_type& stream);

      ~ostream_iterator();

      ostream_iterator& operator=(const T& value);

      ostream_iterator& operator*();

      ostream_iterator& operator++();
      ostream_iterator& operator++(int);

    private:
    };

  } // namespace v1
} // namespace rsl