// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: remove_pointer.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    struct remove_pointer
    {
      using type = T;
    };
    template <typename T>
    struct remove_pointer<T*>
    {
      using type = T;
    };
    template <typename T>
    struct remove_pointer<T* const>
    {
      using type = T;
    };
    template <typename T>
    struct remove_pointer<T* volatile>
    {
      using type = T;
    };
    template <typename T>
    struct remove_pointer<T* const volatile>
    {
      using type = T;
    };

    template <typename T>
    using remove_pointer_t = typename remove_pointer<T>::type;

  } // namespace v1
} // namespace rsl
