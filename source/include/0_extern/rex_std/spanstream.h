// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: spanstream.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#ifdef REX_ENABLE_WITH_CPP23

  #include "rex_std/disable_std_checking.h"
  #include "rex_std/std_alias_defines.h"

  #include <spanstream>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT, typename Traits = char_traits<CharT>>, basic_spanbuf, CharT, Traits);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT, typename Traits = char_traits<CharT>>, basic_ispanstream, CharT, Traits);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT, typename Traits = char_traits<CharT>>, basic_ospanstream, CharT, Traits);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT, typename Traits = char_traits<CharT>>, basic_spanstream, CharT, Traits);

    REX_STD_CLASS_ALIAS(spanbuf);
    REX_STD_CLASS_ALIAS(wspanbuf);

    REX_STD_CLASS_ALIAS(ispanstream);
    REX_STD_CLASS_ALIAS(wispanstream);

    REX_STD_CLASS_ALIAS(ospanstream);
    REX_STD_CLASS_ALIAS(wospanstream);

    REX_STD_CLASS_ALIAS(spanstream);
    REX_STD_CLASS_ALIAS(wspanstream);

    // REX_STD_FUNC_ALIAS(swap); // Doesn't work because rsl has its own swap

  } // namespace v1
} // namespace rsl

  #include "rex_std/enable_std_checking.h"

#endif // REX_ENABLE_WITH_CPP23