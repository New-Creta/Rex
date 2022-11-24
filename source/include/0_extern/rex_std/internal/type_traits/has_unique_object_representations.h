// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: has_unique_object_representations.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/integral_constant.h"
#include "rex_std/internal/type_traits/remove_all_extents.h"
#include "rex_std/internal/type_traits/remove_cv.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    struct has_unique_object_representations : public bool_constant<__has_unique_object_representations(remove_cv_t<remove_all_extents_t<T>>)>
    {
    };

    template <typename T>
    inline constexpr bool has_unique_object_representations_v = has_unique_object_representations<T>::value;

  } // namespace v1
} // namespace rsl
