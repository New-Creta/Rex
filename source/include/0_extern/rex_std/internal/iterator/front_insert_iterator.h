// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: front_insert_iterator.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// https://en.cppreference.com/w/cpp/iterator/front_insert_iterator
//
// Prepends elements to a container for which it was constructed.
//-----------------------------------------------------------------------------

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename Container>
    class front_insert_iterator
    {
    public:
      using iterator_category = output_iterator_tag;
      using value_type        = void;
      using difference_type   = ptrdiff;
      using pointer           = void;
      using reference         = void;
      using container_type    = Container;

      constexpr front_insert_iterator()
          : m_container(nullptr)
      {
      }

      constexpr front_insert_iterator(Container& c)
          : m_container(&c)
      {
      }

      constexpr front_insert_iterator& operator=(const typename Container::value_type& value)
      {
        m_container->push_front(value);

        return *this;
      }
      constexpr front_insert_iterator& operator=(typename Container::value_type&& value)
      {
        m_container->push_front(rsl::move(value));

        return *this;
      }

      constexpr front_insert_iterator& operator*()
      {
        return *this;
      }

      constexpr front_insert_iterator& operator++()
      {
        return *this;
      }
      constexpr front_insert_iterator& operator++(int)
      {
        return *this;
      }

    private:
      Container* m_container;
    };

  } // namespace v1
} // namespace rsl