// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: fstream.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/disable_std_checking.h"
#include "rex_std/internal/string/char_traits.h"
#include "rex_std/std_alias_defines.h"

#include <fstream>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT, typename Traits = char_traits<CharT>>, basic_filebuf, CharT, Traits);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT, typename Traits = char_traits<CharT>>, basic_ifstream, CharT, Traits);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT, typename Traits = char_traits<CharT>>, basic_ofstream, CharT, Traits);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT, typename Traits = char_traits<CharT>>, basic_fstream, CharT, Traits);

    REX_STD_CLASS_ALIAS(filebuf);
    REX_STD_CLASS_ALIAS(wfilebuf);
    REX_STD_CLASS_ALIAS(ifstream);
    REX_STD_CLASS_ALIAS(wifstream);
    REX_STD_CLASS_ALIAS(ofstream);
    REX_STD_CLASS_ALIAS(wofstream);
    REX_STD_CLASS_ALIAS(fstream);
    REX_STD_CLASS_ALIAS(wfstream);

    ////REX_STD_FUNC_ALIAS(swap); // Doesn't work because rsl has its own swap // Doesn't work because rsl has its own swap

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"