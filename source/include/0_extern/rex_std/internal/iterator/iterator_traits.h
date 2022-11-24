// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: iterator_traits.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/internal/iterator/iterator_tags.h"

namespace rsl
{
  inline namespace v1
  {

    /// RSL Comment: Different from ISO C++ Standard at time of writing (17/Aug/2022)
    // in C++20, this structure changed making it more complex to implement.
    // this is not supported in Rex at the moment as it's not needed.
    // iterator traits
    template <typename Iterator>
    struct iterator_traits
    {
      using iterator_category = typename Iterator::iterator_category;
      using value_type        = typename Iterator::value_type;
      using difference_type   = typename Iterator::difference_type;
      using pointer           = typename Iterator::pointer;
      using reference         = typename Iterator::reference;
    };
    template <typename T>
    struct iterator_traits<T*>
    {
      using iterator_category = rsl::random_access_iterator_tag;
      using value_type        = T;
      using difference_type   = ptrdiff;
      using pointer           = T*;
      using reference         = T&;
    };
    template <typename T>
    struct iterator_traits<const T*>
    {
      using iterator_category = rsl::random_access_iterator_tag;
      using value_type        = T;
      using difference_type   = ptrdiff;
      using pointer           = const T*;
      using reference         = const T&;
    };

  } // namespace v1
} // namespace rsl
