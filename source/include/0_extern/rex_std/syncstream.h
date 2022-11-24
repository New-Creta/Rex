// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: syncstream.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#ifdef REX_ENABLE_WITH_CPP20

  #include "rex_std/bonus/defines.h"
  #include "rex_std/disable_std_checking.h"
  #include "rex_std/internal/memory/allocator.h"
  #include "rex_std/internal/string/char_traits.h"
  #include "rex_std/ostream.h"
  #include "rex_std/std_alias_defines.h"

  #include <syncstream>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT, typename Traits = char_traits<CharT>, typename Allocator = typed_allocator<CharT>>, basic_syncbuf, CharT, Traits, Allocator);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CharT, typename Traits = char_traits<CharT>, typename Allocator = typed_allocator<CharT>>, basic_osyncstream, CharT, Traits, Allocator);

    REX_STD_CLASS_ALIAS(syncbuf);
    REX_STD_CLASS_ALIAS(wsyncbuf);
    REX_STD_CLASS_ALIAS(osyncstream);
    REX_STD_CLASS_ALIAS(wosyncstream);

    // REX_STD_FUNC_ALIAS(swap); // Doesn't work because rsl has its own swap

  } // namespace v1
} // namespace rsl

  #include "rex_std/enable_std_checking.h"

#endif // REX_ENABLE_WITH_CPP20
