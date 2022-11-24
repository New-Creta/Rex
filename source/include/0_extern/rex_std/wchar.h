// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: wchar.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/disable_std_checking.h"
#include "rex_std/std_alias_defines.h"

#include <wchar>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_CLASS_ALIAS(mbstate_t);
    REX_STD_CLASS_ALIAS(size_t);

    REX_STD_FUNC_ALIAS(wcscpy);
    REX_STD_FUNC_ALIAS(wcsncpy);
    REX_STD_FUNC_ALIAS(wcscat);
    REX_STD_FUNC_ALIAS(wcsncat);
    REX_STD_FUNC_ALIAS(wcsxfrm);
    REX_STD_FUNC_ALIAS(wcslen);
    REX_STD_FUNC_ALIAS(wcscmp);
    REX_STD_FUNC_ALIAS(wcscoll);
    REX_STD_FUNC_ALIAS(wcschr);
    REX_STD_FUNC_ALIAS(wcsspn);
    REX_STD_FUNC_ALIAS(wcsspn);
    REX_STD_FUNC_ALIAS(wcspbrk);
    REX_STD_FUNC_ALIAS(wcsstr);
    REX_STD_FUNC_ALIAS(wcstok);
    REX_STD_FUNC_ALIAS(wmemcpy);
    REX_STD_FUNC_ALIAS(wmemmove);
    REX_STD_FUNC_ALIAS(wmemcmp);
    REX_STD_FUNC_ALIAS(wmemchr);
    REX_STD_FUNC_ALIAS(wmemset);
    REX_STD_FUNC_ALIAS(mbsinit);
    REX_STD_FUNC_ALIAS(btowc);
    REX_STD_FUNC_ALIAS(wctob);
    REX_STD_FUNC_ALIAS(mbrlen);
    REX_STD_FUNC_ALIAS(mbrtowc);
    REX_STD_FUNC_ALIAS(wcrtomb);
    REX_STD_FUNC_ALIAS(mbsrtowcs);
    REX_STD_FUNC_ALIAS(wcsrtombs);
    REX_STD_FUNC_ALIAS(fgetwc);
    REX_STD_FUNC_ALIAS(fgetws);
    REX_STD_FUNC_ALIAS(fputwc);
    REX_STD_FUNC_ALIAS(fputws);
    REX_STD_FUNC_ALIAS(getwchar);
    REX_STD_FUNC_ALIAS(putwchar);
    REX_STD_FUNC_ALIAS(ungetwc);
    REX_STD_FUNC_ALIAS(fwide);
    REX_STD_FUNC_ALIAS(wscanf);
    REX_STD_FUNC_ALIAS(vwscanf);
    REX_STD_FUNC_ALIAS(wprintf);
    REX_STD_FUNC_ALIAS(vwprintf);
    REX_STD_FUNC_ALIAS(vwprintf);
    REX_STD_FUNC_ALIAS(wcsftime);
    REX_STD_FUNC_ALIAS(wcstol);
    REX_STD_FUNC_ALIAS(wcstoul);
    REX_STD_FUNC_ALIAS(wcstof);

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"
