// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: stdio.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/disable_std_checking.h"
#include "rex_std/std_alias_defines.h"

#include <cstdio>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_CLASS_ALIAS(FILE);
    REX_STD_CLASS_ALIAS(fpos_t);
    REX_STD_CLASS_ALIAS(size_t);

    REX_STD_FUNC_ALIAS(fopen);
    REX_STD_FUNC_ALIAS(freopen);
    REX_STD_FUNC_ALIAS(fclose);
    REX_STD_FUNC_ALIAS(fflush);
    REX_STD_FUNC_ALIAS(setbuf);
    REX_STD_FUNC_ALIAS(setvbuf);
    REX_STD_FUNC_ALIAS(fread);
    REX_STD_FUNC_ALIAS(fwrite);
    REX_STD_FUNC_ALIAS(fgetc);
    REX_STD_FUNC_ALIAS(fgets);
    REX_STD_FUNC_ALIAS(fputc);
    REX_STD_FUNC_ALIAS(putc);
    REX_STD_FUNC_ALIAS(fputs);
    REX_STD_FUNC_ALIAS(getchar);
    REX_STD_FUNC_ALIAS(putchar);
    REX_STD_FUNC_ALIAS(puts);
    REX_STD_FUNC_ALIAS(ungetc);
    REX_STD_FUNC_ALIAS(scanf);
    REX_STD_FUNC_ALIAS(fscanf);
    REX_STD_FUNC_ALIAS(sscanf);
    REX_STD_FUNC_ALIAS(vscanf);
    REX_STD_FUNC_ALIAS(vfscanf);
    REX_STD_FUNC_ALIAS(vsscanf);
    REX_STD_FUNC_ALIAS(printf);
    REX_STD_FUNC_ALIAS(fprintf);
    REX_STD_FUNC_ALIAS(sprintf);
    REX_STD_FUNC_ALIAS(snprintf);
    REX_STD_FUNC_ALIAS(vprintf);
    REX_STD_FUNC_ALIAS(vfprintf);
    REX_STD_FUNC_ALIAS(vsprintf);
    REX_STD_FUNC_ALIAS(vsprintf);
    REX_STD_FUNC_ALIAS(vsnprintf);
    REX_STD_FUNC_ALIAS(ftell);
    REX_STD_FUNC_ALIAS(fgetpos);
    REX_STD_FUNC_ALIAS(fseek);
    REX_STD_FUNC_ALIAS(fsetpos);
    REX_STD_FUNC_ALIAS(rewind);
    REX_STD_FUNC_ALIAS(clearerr);
    REX_STD_FUNC_ALIAS(feof);
    REX_STD_FUNC_ALIAS(ferror);
    REX_STD_FUNC_ALIAS(perror);
    REX_STD_FUNC_ALIAS(remove);
    REX_STD_FUNC_ALIAS(rename);
    REX_STD_FUNC_ALIAS(tmpfile);
    REX_STD_FUNC_ALIAS(tmpnam);

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"
