// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: integer_sequence.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/internal/type_traits/is_integral.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T, T... Ints>
    class integer_sequence
    {
    public:
      using value_type = T;
      static_assert(rsl::is_integral_v<T>, "integer_sequence can only be instantiated with integer types");

      static constexpr card32 size()
      {
        return sizeof...(Ints);
      }
    };

    namespace internal
    {
      // breaking coding guidelines here.
      template <card32 Size, typename IndexSeq>
      struct make_index_sequence;

      // breaking coding guidelines here.
      template <card32 Size, card32... Ints>
      struct make_index_sequence<Size, integer_sequence<card32, Ints...>>
      {
        using type = typename make_index_sequence<Size - 1, integer_sequence<card32, Size - 1, Ints...>>::type;
      };

      // breaking coding guidelines here.
      template <card32... Ints>
      struct make_index_sequence<0, integer_sequence<card32, Ints...>>
      {
        using type = integer_sequence<card32, Ints...>;
      };
    } // namespace internal

    template <card32... Ints>
    using index_sequence = integer_sequence<card32, Ints...>;

    // breaking coding guidelines here.
    template <card32 Size>
    using make_index_sequence = typename internal::make_index_sequence<Size, integer_sequence<card32>>::type;

  } // namespace v1
} // namespace rsl
