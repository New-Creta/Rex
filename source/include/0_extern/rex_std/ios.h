// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: ios.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// https://en.cppreference.com/w/cpp/header/ios
//
// Basic I/O header.
//-----------------------------------------------------------------------------

#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/internal/ios/basic_ios.h"
#include "rex_std/internal/ios/fpos.h"
#include "rex_std/internal/ios/io_errc.h"
#include "rex_std/internal/ios/io_types.h"
#include "rex_std/internal/ios/ios_base.h"
/// [06/Sep/2022] RSL Comment: missing is_error_code_enum specialization

namespace rsl
{
  inline namespace v1
  {

    using ios  = basic_ios<char8>;
    using wios = basic_ios<tchar>;

    /// [06/Sep/2022] RSL Comment: iostream_category
    /// [06/Sep/2022] RSL Comment: make_error_code io_errc specialization
    /// [06/Sep/2022] RSL Comment: make_error_condition io_errc specialization

    ios_base& boolalpha(ios_base& stream);
    ios_base& noboolalpha(ios_base& stream);

    ios_base& showbase(ios_base& stream);
    ios_base& noshowbase(ios_base& stream);

    ios_base& showpoint(ios_base& stream);
    ios_base& noshowpoint(ios_base& stream);

    ios_base& showpos(ios_base& stream);
    ios_base& noshowpos(ios_base& stream);

    ios_base& skipws(ios_base& stream);
    ios_base& noskipws(ios_base& stream);

    ios_base& uppercase(ios_base& stream);
    ios_base& nouppercase(ios_base& stream);

    ios_base& unitbuf(ios_base& stream);
    ios_base& nounitbuf(ios_base& stream);

    // ios_base& internal(ios_base& stream); /// [08/Sep/2022] RSL Comment: collides with internal namespace
    ios_base& left(ios_base& stream);
    ios_base& right(ios_base& stream);

    ios_base& dec(ios_base& stream);
    ios_base& hex(ios_base& stream);
    ios_base& oct(ios_base& stream);

    ios_base& fixed(ios_base& stream);
    ios_base& scientific(ios_base& stream);
    ios_base& hexfloat(ios_base& stream);
    ios_base& defaultfloat(ios_base& stream);

  } // namespace v1
} // namespace rsl
