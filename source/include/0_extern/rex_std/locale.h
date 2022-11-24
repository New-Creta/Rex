// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: locale.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/disable_std_checking.h"
#include "rex_std/iterator.h"
#include "rex_std/std_alias_defines.h"

#include <locale>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_CLASS_ALIAS(locale);

    class allocator;

    // Deprecated in C++17
    // template <typename Codecvt, typename Elem = tchar, typename Wide_alloc = allocator, typename Byte_alloc = allocator>
    // REX_STD_TEMPLATED_CLASS_ALIAS(wstring_convert, Codecvt, Elem, Wide_alloc, Byte_alloc);

    template <typename T>
    class char_traits;

    // Deprecated in C++17
    // template <typename Codecvt, typename Elem = tchar, typename Tr = char_traits<tchar>>
    // REX_STD_TEMPLATED_CLASS_ALIAS(wbuffer_convert, Codecvt, Elem, Tr);

    REX_STD_CLASS_ALIAS(ctype_base);
    REX_STD_CLASS_ALIAS(codecvt_base);
    REX_STD_CLASS_ALIAS(messages_base);
    REX_STD_CLASS_ALIAS(time_base);
    REX_STD_CLASS_ALIAS(money_base);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT>, ctype, CharT);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename InternT, typename ExternT, typename StateT>, codecvt, InternT, ExternT, StateT);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT>, collate, CharT);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT>, messages, CharT);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT, typename InputIt = istreambuf_iterator<CharT>>, time_get, CharT, InputIt);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT, typename OutputIt = ostreambuf_iterator<CharT>>, time_put, CharT, OutputIt);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT, typename InputIt = istreambuf_iterator<CharT>>, num_get, CharT, InputIt);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT, typename OutputIt = ostreambuf_iterator<CharT>>, num_put, CharT, OutputIt);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT>, numpunct, CharT);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT, typename InputIt = istreambuf_iterator<CharT>>, money_get, CharT, InputIt);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT, typename OutputIt = ostreambuf_iterator<CharT>>, money_put, CharT, OutputIt);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT, bool International = false>, moneypunct, CharT, International);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT>, ctype_byname, CharT);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename InternT, typename ExternT, typename State>, codecvt_byname, InternT, ExternT, State);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT>, messages_byname, CharT);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT>, collate_byname, CharT);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT, typename InputIt = istreambuf_iterator<CharT>>, time_get_byname, CharT, InputIt);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT, typename OutputIt = ostreambuf_iterator<CharT>>, time_put_byname, CharT, OutputIt);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT>, numpunct_byname, CharT);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT, bool International = false>, moneypunct_byname, CharT, International);

    REX_STD_FUNC_ALIAS(use_facet);
    REX_STD_FUNC_ALIAS(has_facet);

    REX_STD_FUNC_ALIAS(isspace);
    REX_STD_FUNC_ALIAS(isblank);
    REX_STD_FUNC_ALIAS(iscntrl);
    REX_STD_FUNC_ALIAS(isupper);
    REX_STD_FUNC_ALIAS(islower);
    REX_STD_FUNC_ALIAS(isalpha);
    REX_STD_FUNC_ALIAS(isdigit);
    REX_STD_FUNC_ALIAS(ispunct);
    REX_STD_FUNC_ALIAS(isxdigit);
    REX_STD_FUNC_ALIAS(isalnum);
    REX_STD_FUNC_ALIAS(isprint);
    REX_STD_FUNC_ALIAS(isgraph);

    REX_STD_FUNC_ALIAS(toupper);
    REX_STD_FUNC_ALIAS(tolower);

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"
