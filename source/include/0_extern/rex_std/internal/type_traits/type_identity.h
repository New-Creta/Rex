// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: type_identity.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    struct type_identity
    {
      using type = T;
    };

    template <typename T>
    using type_identity_t = typename type_identity<T>::type;

  } // namespace v1
} // namespace rsl
