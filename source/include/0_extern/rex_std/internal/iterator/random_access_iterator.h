// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: random_access_iterator.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/internal/iterator/iterator_tags.h"

/// [21/Aug/2022] RSL Comment: deprecate this file

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    class random_access_iterator
    {
    public:
      using value_type        = T;
      using pointer           = T*;
      using const_pointer     = const T*;
      using reference         = T&;
      using const_reference   = const T&;
      using iterator_category = rsl::random_access_iterator_tag;
      using difference_type   = int32;

      constexpr explicit random_access_iterator(pointer value = nullptr)
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

      constexpr bool operator==(const random_access_iterator other) const
      {
        return m_value == other.m_value;
      }
      constexpr bool operator!=(const random_access_iterator other) const
      {
        return m_value != other.m_value;
      }

      constexpr bool operator<(const random_access_iterator other) const
      {
        return m_value < other.m_value;
      }
      constexpr bool operator<=(const random_access_iterator other) const
      {
        return m_value <= other.m_value;
      }
      constexpr bool operator>(const random_access_iterator other) const
      {
        return m_value > other.m_value;
      }
      constexpr bool operator>=(const random_access_iterator other) const
      {
        return m_value >= other.m_value;
      }

      constexpr random_access_iterator operator++()
      {
        ++m_value;
        return *this;
      }
      constexpr random_access_iterator operator++(int)
      {
        random_access_iterator tmp = *this;
        operator++();
        return tmp;
      }

      constexpr random_access_iterator operator--()
      {
        m_value--;
        return *this;
      }
      constexpr random_access_iterator operator--(int)
      {
        random_access_iterator tmp = *this;
        operator--();
        return tmp;
      }

      constexpr const_reference operator[](card32 index) const
      {
        return *(m_value + index);
      }
      constexpr reference operator[](card32 index)
      {
        return *(m_value + index);
      }

      constexpr random_access_iterator operator+(card32 index) const
      {
        return random_access_iterator(m_value + index);
      }
      constexpr random_access_iterator operator+=(card32 index)
      {
        m_value += index;
      }

      constexpr random_access_iterator operator-(card32 index) const
      {
        return random_access_iterator(m_value - index);
      }
      constexpr difference_type operator-(random_access_iterator it) const
      {
        return static_cast<difference_type>(m_value - it.m_value);
      }
      constexpr random_access_iterator operator-=(card32 index)
      {
        m_value -= index;
      }

    private:
      pointer m_value;
    };

    template <typename T>
    class const_random_access_iterator
    {
    public:
      using value_type        = T;
      using pointer           = const T*;
      using const_pointer     = const T*;
      using reference         = const T&;
      using const_reference   = const T&;
      using iterator_category = rsl::random_access_iterator_tag;
      using difference_type   = int32;

      constexpr explicit const_random_access_iterator(pointer value = nullptr)
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

      constexpr bool operator==(const const_random_access_iterator other) const
      {
        return m_value == other.m_value;
      }
      constexpr bool operator!=(const const_random_access_iterator other) const
      {
        return m_value != other.m_value;
      }

      constexpr bool operator<(const const_random_access_iterator other) const
      {
        return m_value < other.m_value;
      }
      constexpr bool operator<=(const const_random_access_iterator other) const
      {
        return m_value <= other.m_value;
      }
      constexpr bool operator>(const const_random_access_iterator other) const
      {
        return m_value > other.m_value;
      }
      constexpr bool operator>=(const const_random_access_iterator other) const
      {
        return m_value >= other.m_value;
      }

      constexpr const_random_access_iterator operator++()
      {
        ++m_value;
        return *this;
      }
      constexpr const_random_access_iterator operator++(int)
      {
        const_random_access_iterator tmp = *this;
        operator++();
        return tmp;
      }

      constexpr const_random_access_iterator operator--()
      {
        m_value--;
        return *this;
      }
      constexpr const_random_access_iterator operator--(int)
      {
        const_random_access_iterator tmp = *this;
        operator--();
        return tmp;
      }

      constexpr const_reference operator[](card32 index) const
      {
        return *(m_value + index);
      }
      constexpr const_reference operator[](card32 index)
      {
        return *(m_value + index);
      }

      constexpr const_random_access_iterator operator+(card32 index) const
      {
        return const_random_access_iterator(m_value + index);
      }
      constexpr const_random_access_iterator operator+=(card32 index)
      {
        m_value += index;
        return *this;
      }

      constexpr const_random_access_iterator operator-(card32 index) const
      {
        return const_random_access_iterator(m_value - index);
      }
      constexpr difference_type operator-(const_random_access_iterator it) const
      {
        return static_cast<difference_type>(m_value - it.m_value);
      }
      constexpr const_random_access_iterator operator-=(card32 index)
      {
        m_value -= index;
      }

    private:
      pointer m_value;
    };

  } // namespace v1
} // namespace rsl
