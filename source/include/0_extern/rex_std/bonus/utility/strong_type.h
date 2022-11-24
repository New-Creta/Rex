// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: strong_type.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/conditional.h"

#define REX_CREATE_STRONG_TYPE(Type, UnderlyingType)                                                                                                                                                                                                     \
                                                                                                                                                                                                                                                         \
  class Type                                                                                                                                                                                                                                             \
  {                                                                                                                                                                                                                                                      \
  public:                                                                                                                                                                                                                                                \
    using underlying_type = UnderlyingType;                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                         \
    constexpr explicit Type(underlying_type val)                                                                                                                                                                                                         \
        : m_value(val)                                                                                                                                                                                                                                   \
    {                                                                                                                                                                                                                                                    \
    }                                                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                         \
    constexpr underlying_type& get()                                                                                                                                                                                                                     \
    {                                                                                                                                                                                                                                                    \
      return m_value;                                                                                                                                                                                                                                    \
    }                                                                                                                                                                                                                                                    \
    constexpr const underlying_type& get() const                                                                                                                                                                                                         \
    {                                                                                                                                                                                                                                                    \
      return m_value;                                                                                                                                                                                                                                    \
    }                                                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                         \
    constexpr bool operator==(const Type& other) const                                                                                                                                                                                                   \
    {                                                                                                                                                                                                                                                    \
      return m_value == other.m_value;                                                                                                                                                                                                                   \
    }                                                                                                                                                                                                                                                    \
    constexpr bool operator!=(const Type& other) const                                                                                                                                                                                                   \
    {                                                                                                                                                                                                                                                    \
      return m_value != other.m_value;                                                                                                                                                                                                                   \
    }                                                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                         \
  private:                                                                                                                                                                                                                                               \
    underlying_type m_value;                                                                                                                                                                                                                             \
  };
namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename T>
      struct underlying_type
      {
        using type = typename T::underlying_type;
      };

      template <typename T>
      using underlying_type_t = typename underlying_type<T>::type;

      template <typename T>
      struct literal_type
      {
        using type = void;
      };
      template <>
      struct literal_type<unsigned char>
      {
        using type = unsigned long long;
      };
      template <>
      struct literal_type<signed char>
      {
        using type = unsigned long long;
      };
      template <>
      struct literal_type<char>
      {
        using type = unsigned long long;
      };
      template <>
      struct literal_type<short>
      {
        using type = unsigned long long;
      };
      template <>
      struct literal_type<int>
      {
        using type = unsigned long long;
      };
      template <>
      struct literal_type<long>
      {
        using type = unsigned long long;
      };
      template <>
      struct literal_type<long long>
      {
        using type = unsigned long long;
      };
      template <>
      struct literal_type<float>
      {
        using type = long double;
      };
      template <>
      struct literal_type<double>
      {
        using type = long double;
      };
      template <>
      struct literal_type<long double>
      {
        using type = long double;
      };

      template <typename T>
      using literal_type_t = typename literal_type<T>::type;
    } // namespace internal

  } // namespace v1
} // namespace rsl

#define REX_CREATE_LITERAL_FOR_STRONG_TYPE(type, lit_suffix)                                                                                                                                                                                             \
  constexpr type operator"" _##lit_suffix(rsl::internal::literal_type_t<rsl::internal::underlying_type_t<type>> value)                                                                                                                                   \
  {                                                                                                                                                                                                                                                      \
    return type(static_cast<rsl::internal::underlying_type_t<type>>(value));                                                                                                                                                                             \
  }
