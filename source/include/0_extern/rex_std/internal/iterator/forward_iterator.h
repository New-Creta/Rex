// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: forward_iterator.h
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
    class forward_iterator
    {
    public:
      using value_type        = T;
      using pointer           = T*;
      using const_pointer     = const T*;
      using reference         = T&;
      using const_reference   = const T&;
      using iterator_category = rsl::forward_iterator_tag;
      using difference_type   = int32;

      constexpr explicit forward_iterator(pointer value = nullptr)
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

      constexpr bool operator==(const forward_iterator other) const
      {
        return m_value == other.m_value;
      }
      constexpr bool operator!=(const forward_iterator other) const
      {
        return m_value != other.m_value;
      }

      constexpr bool operator<(const forward_iterator other) const
      {
        return m_value < other.m_value;
      }
      constexpr bool operator<=(const forward_iterator other) const
      {
        return m_value <= other.m_value;
      }
      constexpr bool operator>(const forward_iterator other) const
      {
        return m_value > other.m_value;
      }
      constexpr bool operator>=(const forward_iterator other) const
      {
        return m_value >= other.m_value;
      }

      constexpr forward_iterator operator++()
      {
        ++m_value;
        return *this;
      }
      constexpr forward_iterator operator++(int)
      {
        forward_iterator tmp = *this;
        operator++();
        return tmp;
      }

    private:
      pointer m_value;
    };

    template <typename T>
    class const_forward_iterator
    {
    public:
      using value_type        = T;
      using pointer           = T*;
      using const_pointer     = const T*;
      using reference         = const T&;
      using const_reference   = const T&;
      using iterator_category = rsl::forward_iterator_tag;
      using difference_type   = int32;

      constexpr explicit const_forward_iterator(pointer value = nullptr)
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

      constexpr bool operator==(const const_forward_iterator other) const
      {
        return m_value == other.m_value;
      }
      constexpr bool operator!=(const const_forward_iterator other) const
      {
        return m_value != other.m_value;
      }

      constexpr bool operator<(const const_forward_iterator other) const
      {
        return m_value < other.m_value;
      }
      constexpr bool operator<=(const const_forward_iterator other) const
      {
        return m_value <= other.m_value;
      }
      constexpr bool operator>(const const_forward_iterator other) const
      {
        return m_value > other.m_value;
      }
      constexpr bool operator>=(const const_forward_iterator other) const
      {
        return m_value >= other.m_value;
      }

      constexpr const_forward_iterator operator++()
      {
        ++m_value;
        return *this;
      }
      constexpr const_forward_iterator operator++(int)
      {
        const_forward_iterator tmp = *this;
        operator++();
        return tmp;
      }

    private:
      pointer m_value;
    };

  } // namespace v1
} // namespace rsl
