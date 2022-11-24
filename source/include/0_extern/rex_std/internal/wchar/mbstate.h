// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: mbstate.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// https://en.cppreference.com/w/cpp/string/multibyte/mbstate_t
//
// A type that represent any of the conversion states that can
// occur in a set of supported multibyte character encoding rules
// zero initialized value of mbstate_t represents the initial conversion state
//
// a mbstate should be used with function calls to mbrlen, mbrtowc, mbsrtowcs,
// mbtowc, wcrtomb, wcsrtombs, wctomb.
//-----------------------------------------------------------------------------

#pragma once

#include "rex_std/bonus/types.h"

namespace rsl
{
  inline namespace v1
  {

    struct mbstate_t
    {
      ulong wchar; // the byte to convert
      uint16 byte;
      uint16 state; // the current state of the conversion
    };

  } // namespace v1
} // namespace rsl