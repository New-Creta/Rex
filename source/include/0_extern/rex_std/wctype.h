// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: wctype.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/defines.h"
#include "rex_std/disable_std_checking.h"
#include "rex_std/std_alias_defines.h"

#include <cwctype>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_CLASS_ALIAS(wctrans_t);
    REX_STD_CLASS_ALIAS(wctype_t);

    REX_STD_FUNC_ALIAS(iswalnum);
    REX_STD_FUNC_ALIAS(iswalpha);
    REX_STD_FUNC_ALIAS(iswlower);
    REX_STD_FUNC_ALIAS(iswupper);
    REX_STD_FUNC_ALIAS(iswdigit);
    REX_STD_FUNC_ALIAS(iswxdigit);
    REX_STD_FUNC_ALIAS(iswcntrl);
    REX_STD_FUNC_ALIAS(iswgraph);
    REX_STD_FUNC_ALIAS(iswspace);
    REX_STD_FUNC_ALIAS(iswblank);
    REX_STD_FUNC_ALIAS(iswprint);
    REX_STD_FUNC_ALIAS(iswpunct);
    REX_STD_FUNC_ALIAS(iswctype);
    REX_STD_FUNC_ALIAS(wctype);
    REX_STD_FUNC_ALIAS(towlower);
    REX_STD_FUNC_ALIAS(towupper);
    REX_STD_FUNC_ALIAS(wctrans);
    REX_STD_FUNC_ALIAS(towctrans);

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"
