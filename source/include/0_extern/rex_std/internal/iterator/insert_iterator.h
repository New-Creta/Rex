// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: insert_iterator.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// https://en.cppreference.com/w/cpp/iterator/insert_iterator/insert_iterator
//
// Inserts elements into a container for which it was constructed
//-----------------------------------------------------------------------------

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename Container>
    class insert_iterator
    {
    public:
      using iterator_category = output_iterator_tag;
      using value_type        = void;
      using difference_type   = ptrdiff;
      using pointer           = void;
      using reference         = void;
      using container_type    = Container;

      constexpr insert_iterator()
          : m_container(nullptr)
          , m_it()
      {
      }

      constexpr insert_iterator(Container& c, typename Container::iterator it)
          : m_container(&c)
          , m_it(it)
      {
      }

      constexpr insert_iterator& operator=(const typename Container::value_type& value)
      {
        m_it = m_container->insert(m_it, value);
        ++m_it;

        return *this;
      }
      constexpr insert_iterator& operator=(typename Container::value_type&& value)
      {
        m_it = m_container->insert(m_it, rsl::move(value));
        ++m_it;

        return *this;
      }

      constexpr insert_iterator& operator*()
      {
        return *this;
      }

      constexpr insert_iterator& operator++()
      {
        return *this;
      }
      constexpr insert_iterator& operator++(int)
      {
        return *this;
      }

    private:
      Container* m_container;
      typename Container::iterator m_it;
    };

  } // namespace v1
} // namespace rsl