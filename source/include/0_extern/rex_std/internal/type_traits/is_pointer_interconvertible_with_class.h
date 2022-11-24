// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_pointer_interconvertible_with_class.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#ifdef REX_ENABLE_WITH_CPP20

namespace rsl
{
  inline namespace v1
  {

    template <typename S, typename M>
    inline constexpr bool is_pointer_interconvertible_with_class(M S::*mp)
    {
      return __is_pointer_interconvertible_with_class(S, M);
    }

  } // namespace v1
} // namespace rsl

#endif