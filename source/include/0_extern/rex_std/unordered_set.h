// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: unordered_set.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/defines.h"
#include "rex_std/bonus/memory/typed_allocator.h"
#include "rex_std/compare.h"
#include "rex_std/disable_std_checking.h"
#include "rex_std/initializer_list.h"
#include "rex_std/std_alias_defines.h"

#include <unordered_set>

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    struct hash;

    template <typename T>
    struct equal_to;

    class allocator;

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename Key, typename Hash = hash<Key>, typename KeyEqual = equal_to<Key>, typename Allocator = typed_allocator<Key>>, unordered_set, Key, Hash, KeyEqual, Allocator);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename Key, typename Hash = hash<Key>, typename KeyEqual = equal_to<Key>, typename Allocator = typed_allocator<Key>>, unordered_multiset, Key, Hash, KeyEqual, Allocator);

    REX_STD_FUNC_ALIAS(operator==);
    REX_STD_FUNC_ALIAS(operator!=);

    REX_STD_FUNC_ALIAS(erase_if);

    REX_STD_FUNC_ALIAS(begin);
    REX_STD_FUNC_ALIAS(cbegin);
    REX_STD_FUNC_ALIAS(cend);
    REX_STD_FUNC_ALIAS(rbegin);
    REX_STD_FUNC_ALIAS(crbegin);
    REX_STD_FUNC_ALIAS(rend);
    REX_STD_FUNC_ALIAS(crend);
    REX_STD_FUNC_ALIAS(size);
    REX_STD_FUNC_ALIAS(ssize);
    REX_STD_FUNC_ALIAS(empty);
    REX_STD_FUNC_ALIAS(data);

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"
