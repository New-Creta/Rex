// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: conjunction.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/integral_constant.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      // handle false trait or last trait
      template <bool FirstValue, typename First, typename... Rest>
      struct conjunction_helper
      {
        using type = First;
      };

      // the first trait is true, try the next one
      template <typename True, typename Next, typename... Rest>
      struct conjunction_helper<true, True, Next, Rest...>
      {
        using type = typename conjunction_helper<Next::value, Next, Rest...>::type;
      };
    } // namespace internal

    template <typename... Traits>
    struct conjunction : true_type
    {
    }; // If _Traits is empty, true_type

    template <typename First, typename... Rest>
    struct conjunction<First, Rest...> : internal::conjunction_helper<First::value, First, Rest...>::type
    {
    };

    template <typename... Traits>
    inline constexpr bool conjunction_v = conjunction<Traits...>::value;

  } // namespace v1
} // namespace rsl
