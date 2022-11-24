// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: hash_node.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/utility/move.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename Value>
    struct hash_node
    {
      hash_node() = default;
      explicit hash_node(const Value& valueParam)
          : value(valueParam)
          , next(nullptr)
      {
      }
      explicit hash_node(Value&& valueParam)
          : value(rsl::move(valueParam))
          , next(nullptr)
      {
      }
      hash_node(const hash_node&)            = default;
      hash_node(hash_node&&)                 = default;
      ~hash_node()                           = default;
      hash_node& operator=(const hash_node&) = default;
      hash_node& operator=(hash_node&&)      = default;

      Value value;
      hash_node* next;
    };

  } // namespace v1
} // namespace rsl