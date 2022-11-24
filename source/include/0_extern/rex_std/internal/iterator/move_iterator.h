// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: move_iterator.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/iterator/iterator_traits.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename Iter>
    class move_iterator
    {
    public:
      using iterator_type     = Iter;
      using iterator_category = typename iterator_traits<Iter>::iterator_category;
      using value_type        = typename iterator_traits<Iter>::value_type;
      using difference_type   = typename iterator_traits<Iter>::difference_type;
      using pointer           = Iter;
      using reference         = typename iterator_traits<Iter>::reference;

      constexpr move_iterator();
      constexpr explicit move_iterator(iterator_type it);
      template <typename U>
      constexpr explicit move_iterator(const move_iterator<U>& other);

      template <typename U>
      constexpr move_iterator& operator=(const move_iterator<U>& other);

      constexpr const iterator_type& base() const&;
      constexpr iterator_type base() &&;

      constexpr reference operator*() const;
      constexpr pointer operator->() const;

      constexpr auto operator[](difference_type n) const;

      constexpr move_iterator& operator++();
      constexpr move_iterator& operator--();

      constexpr move_iterator operator++(int);
      constexpr move_iterator operator--(int);

      constexpr move_iterator operator+(difference_type n) const;
      constexpr move_iterator operator-(difference_type n) const;
      constexpr move_iterator& operator+=(difference_type n);
      constexpr move_iterator& operator-=(difference_type n);

    private:
      iterator_type m_current;
    };

    template <typename Iterator1, typename Iterator2>
    constexpr bool operator==(const move_iterator<Iterator1>& lhs, const move_iterator<Iterator2>& rhs);
    template <typename Iterator1, typename Iterator2>
    constexpr bool operator!=(const move_iterator<Iterator1>& lhs, const move_iterator<Iterator2>& rhs);

    template <typename Iterator1, typename Iterator2>
    constexpr bool operator<(const move_iterator<Iterator1>& lhs, const move_iterator<Iterator2>& rhs);
    template <typename Iterator1, typename Iterator2>
    constexpr bool operator<=(const move_iterator<Iterator1>& lhs, const move_iterator<Iterator2>& rhs);
    template <typename Iterator1, typename Iterator2>
    constexpr bool operator>(const move_iterator<Iterator1>& lhs, const move_iterator<Iterator2>& rhs);
    template <typename Iterator1, typename Iterator2>
    constexpr bool operator>=(const move_iterator<Iterator1>& lhs, const move_iterator<Iterator2>& rhs);

    template <typename Iter>
    constexpr move_iterator<Iter> operator+(typename move_iterator<Iter>::difference_type n, const move_iterator<Iter>& it);

    template <typename Iterator1, typename Iterator2>
    constexpr auto operator-(const move_iterator<Iterator1>& lhs, const move_iterator<Iterator2>& rhs) -> decltype(lhs.base() - rhs.base());

    template <typename Iter>
    constexpr move_iterator<Iter> make_move_iterator(Iter i);

  } // namespace v1
} // namespace rsl
