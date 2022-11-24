// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: initializer_list.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/iterator/reverse_iterator.h"

#ifndef _INITIALIZER_LIST_ // MSVC STD Include guard
  #define _INITIALIZER_LIST_
namespace std // needs to be in std namespace
{
  // See the C++11 Standard, section 18.9.
  // This is MSVC's version of initializer_list.
  // it's possible another compiler will need another implementation of init list
  template <typename Elem>
  class initializer_list
  {
  public:
    using value_type      = Elem;
    using reference       = const Elem&;
    using const_reference = const Elem&;
    using size_type       = size_t;

    using iterator       = const Elem*;
    using const_iterator = const Elem*;

    constexpr initializer_list() noexcept
        : m_first(nullptr)
        , m_last(nullptr)
    {
    }

    constexpr initializer_list(const Elem* firstArg, const Elem* lastArg) noexcept
        : m_first(firstArg)
        , m_last(lastArg)
    {
    }

    [[nodiscard]] constexpr const Elem* begin() const noexcept
    {
      return m_first;
    }

    [[nodiscard]] constexpr const Elem* end() const noexcept
    {
      return m_last;
    }

    [[nodiscard]] constexpr size_t size() const noexcept
    {
      return static_cast<size_t>(m_last - m_first);
    }

  private:
    const Elem* m_first;
    const Elem* m_last;
  };
} // namespace std

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    using initializer_list = std::initializer_list<T>;

    template <typename T>
    constexpr const T* begin(rsl::initializer_list<T> ilist) noexcept
    {
      return ilist.begin();
    }

    template <typename T>
    constexpr const T* end(rsl::initializer_list<T> ilist) noexcept
    {
      return ilist.end();
    }

    template <typename T>
    reverse_iterator<typename initializer_list<T>::iterator> rbegin(initializer_list<T> ilist)
    {
      return reverse_iterator<typename initializer_list<T>::iterator>(ilist.end());
    }
    template <typename T>
    reverse_iterator<typename initializer_list<T>::const_iterator> crbegin(initializer_list<T> ilist)
    {
      return reverse_iterator<typename initializer_list<T>::iterator>(ilist.end());
    }

    template <typename T>
    reverse_iterator<typename initializer_list<T>::iterator> rend(initializer_list<T> ilist)
    {
      return reverse_iterator<typename initializer_list<T>::iterator>(ilist.begin());
    }
    template <typename T>
    reverse_iterator<typename initializer_list<T>::const_iterator> crend(initializer_list<T> ilist)
    {
      return reverse_iterator<typename initializer_list<T>::iterator>(ilist.begin());
    }

    template <typename T>
    constexpr bool empty(rsl::initializer_list<T> ilist)
    {
      return ilist.size() == 0;
    }

    template <typename T>
    constexpr T* data(rsl::initializer_list<T> ilist)
    {
      return ilist.begin();
    }

  } // namespace v1
} // namespace rsl
#endif

namespace rsl
{
  inline namespace v1
  {

    // needs to be defined again here, in case std::intializer_list is already included in another header
    template <typename T>
    using initializer_list = std::initializer_list<T>;

  } // namespace v1
} // namespace rsl
