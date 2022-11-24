// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: remove_cvref.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/remove_cv.h"
#include "rex_std/internal/type_traits/remove_reference.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename T>
      using RemoveCVRefHelper = remove_cv_t<remove_reference_t<T>>;
    } // namespace internal

    template <typename T>
    using remove_cvref_t = internal::RemoveCVRefHelper<T>;

    template <typename T>
    struct remove_cvref
    {
      using type = remove_cvref_t<T>;
    };

  } // namespace v1
} // namespace rsl
