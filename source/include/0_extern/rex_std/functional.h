// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: functional.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

// This header heavily differs from the C++ Standard.
// main reason for this is because the C++ standard needs to remain
// backwards compatible, they haven't removed functions like std::bind.
// While rsl is starting from scratch, we don't have support legacy code
// which is generally agreed to be poor code to use.

#pragma once

#include "rex_std/internal/functional/bit_and.h"
#include "rex_std/internal/functional/bit_not.h"
#include "rex_std/internal/functional/bit_or.h"
#include "rex_std/internal/functional/bit_xor.h"
#include "rex_std/internal/functional/divides.h"
#include "rex_std/internal/functional/equal_to.h"
#include "rex_std/internal/functional/function.h"
#include "rex_std/internal/functional/greater.h"
#include "rex_std/internal/functional/greater_equal.h"
#include "rex_std/internal/functional/hash.h"
#include "rex_std/internal/functional/less.h"
#include "rex_std/internal/functional/less_equal.h"
#include "rex_std/internal/functional/logical_and.h"
#include "rex_std/internal/functional/logical_not.h"
#include "rex_std/internal/functional/logical_or.h"
#include "rex_std/internal/functional/minus.h"
#include "rex_std/internal/functional/modulus.h"
#include "rex_std/internal/functional/multiplies.h"
#include "rex_std/internal/functional/negate.h"
#include "rex_std/internal/functional/not_equal_to.h"
#include "rex_std/internal/functional/not_fn.h"
#include "rex_std/internal/functional/plus.h"
#include "rex_std/internal/iterator/iterator_traits.h"

/// RSL Comment: Different from ISO C++ Standard at time of writing (17/Jul/2022)
// the placeholders namespace is not defined in rsl as it is mainly used with std::bind.
// namespace placeholders {}

#include "rex_std/disable_std_checking.h"
#include "rex_std/std_alias_defines.h"

#include <functional>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_FUNC_ALIAS(mem_fn);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename ForwardIt, typename BinaryPredicate = rsl::equal_to<>>, default_searcher, ForwardIt, BinaryPredicate);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename RandomIt1, typename Hash = rsl::hash<typename rsl::iterator_traits<RandomIt1>::value_type>, typename BinaryPredicate = rsl::equal_to<>>, boyer_moore_searcher, RandomIt1, Hash, BinaryPredicate);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename RandomIt1, typename Hash = rsl::hash<typename rsl::iterator_traits<RandomIt1>::value_type>, typename BinaryPredicate = rsl::equal_to<>>, boyer_moore_horspool_searcher, RandomIt1, Hash,
                                  BinaryPredicate);

    // Leaving out bind_front, bind_back and bind because people should be using lambdas

#ifdef REX_ENABLE_WITH_CPP23

    REX_STD_TEMPLATED_CLASS_ALIAS(move_only_function, typename...);

    REX_STD_FUNC_ALIAS(invoke_r);

#endif // REX_ENABLE_WITH_CPP23

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"