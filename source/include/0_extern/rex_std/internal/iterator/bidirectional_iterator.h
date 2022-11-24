// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: bidirectional_iterator.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/iterator/iterator_tags.h"

/// [21/Aug/2022] RSL Comment: deprecate this file

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    class bi_directional_iterator
    {
    public:
      using value_type        = T;
      using pointer           = T*;
      using const_pointer     = const T*;
      using reference         = T&;
      using const_reference   = const T&;
      using iterator_category = rsl::bidirectional_iterator_tag;
      using difference_type   = int32;

      constexpr explicit bi_directional_iterator(pointer value = nullptr)
          : m_value(value)
      {
      }

      constexpr reference operator*()
      {
        return *m_value;
      }
      constexpr pointer operator->()
      {
        return m_value;
      }

      constexpr const_reference operator*() const
      {
        return *m_value;
      }
      constexpr const_pointer operator->() const
      {
        return m_value;
      }

      constexpr bool operator==(const bi_directional_iterator other) const
      {
        return m_value == other.m_value;
      }
      constexpr bool operator!=(const bi_directional_iterator other) const
      {
        return m_value != other.m_value;
      }

      constexpr bool operator<(const bi_directional_iterator other) const
      {
        return m_value < other.m_value;
      }
      constexpr bool operator<=(const bi_directional_iterator other) const
      {
        return m_value <= other.m_value;
      }
      constexpr bool operator>(const bi_directional_iterator other) const
      {
        return m_value > other.m_value;
      }
      constexpr bool operator>=(const bi_directional_iterator other) const
      {
        return m_value >= other.m_value;
      }

      constexpr bi_directional_iterator operator++()
      {
        ++m_value;
        return *this;
      }
      constexpr bi_directional_iterator operator++(int)
      {
        bi_directional_iterator tmp = *this;
        operator++();
        return tmp;
      }

      constexpr bi_directional_iterator operator--()
      {
        m_value--;
        return *this;
      }
      constexpr bi_directional_iterator operator--(int)
      {
        bi_directional_iterator tmp = *this;
        operator--();
        return tmp;
      }

    private:
      pointer m_value;
    };

    template <typename T>
    class const_bi_directional_iterator
    {
    public:
      using value_type        = T;
      using pointer           = T*;
      using const_pointer     = const T*;
      using reference         = const T&;
      using const_reference   = const T&;
      using iterator_category = rsl::bidirectional_iterator_tag;
      using difference_type   = int32;

      constexpr explicit const_bi_directional_iterator(pointer value = nullptr)
          : m_value(value)
      {
      }

      constexpr const_reference operator*() const
      {
        return *m_value;
      }
      constexpr const_pointer operator->() const
      {
        return m_value;
      }

      constexpr bool operator==(const const_bi_directional_iterator other) const
      {
        return m_value == other.m_value;
      }
      constexpr bool operator!=(const const_bi_directional_iterator other) const
      {
        return m_value != other.m_value;
      }

      constexpr bool operator<(const const_bi_directional_iterator other) const
      {
        return m_value < other.m_value;
      }
      constexpr bool operator<=(const const_bi_directional_iterator other) const
      {
        return m_value <= other.m_value;
      }
      constexpr bool operator>(const const_bi_directional_iterator other) const
      {
        return m_value > other.m_value;
      }
      constexpr bool operator>=(const const_bi_directional_iterator other) const
      {
        return m_value >= other.m_value;
      }

      constexpr const_bi_directional_iterator operator++()
      {
        ++m_value;
        return *this;
      }
      constexpr const_bi_directional_iterator operator++(int)
      {
        const_bi_directional_iterator tmp = *this;
        operator++();
        return tmp;
      }

      constexpr const_bi_directional_iterator operator--()
      {
        m_value--;
        return *this;
      }
      constexpr const_bi_directional_iterator operator--(int)
      {
        const_bi_directional_iterator tmp = *this;
        operator--();
        return tmp;
      }

    private:
      pointer m_value;
    };

  } // namespace v1
} // namespace rsl
