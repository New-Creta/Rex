// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: complex.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/disable_std_checking.h"
#include "rex_std/std_alias_defines.h"

#include <complex>

namespace rsl
{
  inline namespace v1
  {
    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename T>, complex, T);

    REX_STD_FUNC_ALIAS(operator+);
    REX_STD_FUNC_ALIAS(operator-);
    REX_STD_FUNC_ALIAS(operator*);
    REX_STD_FUNC_ALIAS(operator/);
    REX_STD_FUNC_ALIAS(operator==);
    REX_STD_FUNC_ALIAS(operator!=);
    REX_STD_FUNC_ALIAS(operator<<);
    REX_STD_FUNC_ALIAS(operator>>);
    REX_STD_FUNC_ALIAS(real);
    REX_STD_FUNC_ALIAS(imag);
    // REX_STD_FUNC_ALIAS(abs); // doesn't work because we have a rsl version
    REX_STD_FUNC_ALIAS(norm);
    REX_STD_FUNC_ALIAS(conj);
    REX_STD_FUNC_ALIAS(proj);
    REX_STD_FUNC_ALIAS(polar);

    REX_STD_FUNC_ALIAS(exp);
    REX_STD_FUNC_ALIAS(log);
    REX_STD_FUNC_ALIAS(log10);

    REX_STD_FUNC_ALIAS(pow);
    REX_STD_FUNC_ALIAS(sqrt);

    REX_STD_FUNC_ALIAS(sin);
    REX_STD_FUNC_ALIAS(cos);
    REX_STD_FUNC_ALIAS(tan);
    REX_STD_FUNC_ALIAS(asin);
    REX_STD_FUNC_ALIAS(acos);
    REX_STD_FUNC_ALIAS(atan);

    REX_STD_FUNC_ALIAS(sinh);
    REX_STD_FUNC_ALIAS(sinh);
    REX_STD_FUNC_ALIAS(tanh);
    REX_STD_FUNC_ALIAS(asinh);
    REX_STD_FUNC_ALIAS(acosh);
    REX_STD_FUNC_ALIAS(atanh);

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"
