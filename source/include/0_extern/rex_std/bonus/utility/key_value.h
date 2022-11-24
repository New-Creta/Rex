// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: key_value.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename Key, typename Value>
    struct key_value
    {
    public:
      using key_type   = Key;
      using value_type = Value;

      // useful type alias for pair utilities
      using first_type  = key_type;
      using second_type = value_type;

      const Key& first() const
      {
        return key;
      }
      Key& first()
      {
        return key;
      }

      const Value& second() const
      {
        return value;
      }
      Value& second()
      {
        return value;
      }

    public:
      Key key; // NOLINT(readability-identifier-naming, cppcoreguidelines-avoid-const-or-ref-data-members)
      Value value;
    };

  } // namespace v1
} // namespace rsl
