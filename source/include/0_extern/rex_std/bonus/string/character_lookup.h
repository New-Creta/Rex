// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: character_lookup.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/array.h"
#include "rex_std/internal/iterator/random_access_iterator.h"
#include "rex_std/internal/iterator/reverse_iterator.h"
#include "rex_std/internal/type_traits/make_unsigned.h"
#include "rex_std/limits.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename CharType>
    class character_lookup
    {
    public:
      using value_type             = CharType;
      using pointer                = value_type*;
      using const_pointer          = const value_type*;
      using reference              = value_type&;
      using const_reference        = const value_type&;
      using const_iterator         = const_random_access_iterator<value_type>;
      using iterator               = const_iterator;
      using const_reverse_iterator = reverse_iterator<const_iterator>;
      using reverse_iteartor       = const_reverse_iterator;
      using size_type              = count_t;
      using difference_type        = int32;

      constexpr character_lookup(const_pointer str, size_type length)
          : m_table()
      {
        for(size_type i = 0; i < length; ++i)
        {
          const value_type c = str[i];
          m_table[c]         = true;
        }
      }

      bool exists(value_type c) const
      {
        return m_table[c];
      }

    private:
      rsl::array<bool, (numeric_limits<rsl::make_unsigned_t<CharType>>::max)() + 1> m_table {}; // NOLINT
    };

  } // namespace v1
} // namespace rsl
