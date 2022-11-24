// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: regex.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/memory/typed_allocator.h"
#include "rex_std/disable_std_checking.h"
#include "rex_std/internal/iterator/iterator_traits.h"
#include "rex_std/std_alias_defines.h"

#include <regex>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT>, regex_traits, CharT);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT, typename Traits = regex_traits<CharT>>, basic_regex, CharT, Traits);

    REX_STD_CLASS_ALIAS(regex);
    REX_STD_CLASS_ALIAS(wregex);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename BiDirIt>, sub_match, BiDirIt);

    REX_STD_CLASS_ALIAS(csub_match);
    REX_STD_CLASS_ALIAS(wcsub_match);
    REX_STD_CLASS_ALIAS(ssub_match);
    REX_STD_CLASS_ALIAS(wssub_match);

    class allocator;

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename BiDirIt, typename Alloc = typed_allocator<typename iterator_traits<BiDirIt>::value_type>>, match_results, BiDirIt, Alloc);

    REX_STD_CLASS_ALIAS(cmatch);
    REX_STD_CLASS_ALIAS(wcmatch);
    REX_STD_CLASS_ALIAS(smatch);
    REX_STD_CLASS_ALIAS(wsmatch);

    template <typename Iterator>
    struct iterator_traits;

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename BiDirIt, typename CharT, typename iterator_traits<BiDirIt>::value_type, typename Traits = regex_traits<CharT>>, regex_iterator, BiDirIt, CharT, Traits);

    REX_STD_CLASS_ALIAS(cregex_iterator);
    REX_STD_CLASS_ALIAS(wcregex_iterator);
    REX_STD_CLASS_ALIAS(sregex_iterator);
    REX_STD_CLASS_ALIAS(wsregex_iterator);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename BiDirIt, typename CharT = typename iterator_traits<BiDirIt>::value_type, typename Traits = regex_traits<CharT>>, regex_token_iterator, BiDirIt, CharT, Traits);

    REX_STD_CLASS_ALIAS(cregex_token_iterator);
    REX_STD_CLASS_ALIAS(wcregex_token_iterator);
    REX_STD_CLASS_ALIAS(sregex_token_iterator);
    REX_STD_CLASS_ALIAS(wsregex_token_iterator);

    REX_STD_CLASS_ALIAS(regex_error);

    namespace regex_constants
    {
      using namespace std::regex_constants;
    }

    REX_STD_FUNC_ALIAS(regex_match);
    REX_STD_FUNC_ALIAS(regex_search);
    REX_STD_FUNC_ALIAS(regex_replace);

    // REX_STD_FUNC_ALIAS(swap); // Doesn't work because rsl has its own swap

    REX_STD_FUNC_ALIAS(operator==);
    REX_STD_FUNC_ALIAS(operator!=);
    REX_STD_FUNC_ALIAS(operator<);
    REX_STD_FUNC_ALIAS(operator<=);
    REX_STD_FUNC_ALIAS(operator>);
    REX_STD_FUNC_ALIAS(operator>=);
    REX_STD_FUNC_ALIAS(operator<<);
    REX_STD_FUNC_ALIAS(begin);
    REX_STD_FUNC_ALIAS(cbegin);
    REX_STD_FUNC_ALIAS(cend);
    REX_STD_FUNC_ALIAS(rbegin);
    REX_STD_FUNC_ALIAS(crbegin);
    REX_STD_FUNC_ALIAS(rend);
    REX_STD_FUNC_ALIAS(crend);
    REX_STD_FUNC_ALIAS(size);
    REX_STD_FUNC_ALIAS(ssize);
    REX_STD_FUNC_ALIAS(empty);
    REX_STD_FUNC_ALIAS(data);

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"
