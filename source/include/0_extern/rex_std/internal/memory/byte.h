// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: byte.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/internal/type_traits/enable_if.h"
#include "rex_std/internal/type_traits/is_integral.h"

namespace rsl
{
  inline namespace v1
  {

    // distinct type that implements the concept of byte
    enum class byte : uint8
    {
    };
    static_assert(sizeof(byte) == 1, "byte is not 1 byte big");

    // bitwise LEFT SHIFT, every static_cast is intentional
    template <typename IntType, enable_if_t<is_integral_v<IntType>, bool> = true>
    REX_NO_DISCARD constexpr byte operator<<(const byte arg, const IntType shift) noexcept
    {
      return static_cast<byte>(static_cast<uint8>(static_cast<uint32>(arg) << shift));
    }

    // bitwise RIGHT SHIFT, every static_cast is intentional
    template <typename IntType, enable_if_t<is_integral_v<IntType>, bool> = true>
    REX_NO_DISCARD constexpr byte operator>>(const byte arg, const IntType shift) noexcept
    {
      return static_cast<byte>(static_cast<uint8>(static_cast<uint8>(arg) >> shift));
    }

    // bitwise OR, every static_cast is intentional
    REX_NO_DISCARD constexpr byte operator|(const byte left, const byte right) noexcept
    {
      return static_cast<byte>(static_cast<uint8>(static_cast<uint8>(left) | static_cast<uint8>(right)));
    }

    // bitwise AND, every static_cast is intentional
    REX_NO_DISCARD constexpr byte operator&(const byte left, const byte right) noexcept
    {
      return static_cast<byte>(static_cast<uint8>(static_cast<uint8>(left) & static_cast<uint8>(right)));
    }

    // bitwise XOR, every static_cast is intentional
    REX_NO_DISCARD constexpr byte operator^(const byte left, const byte right) noexcept
    {
      return static_cast<byte>(static_cast<uint8>(static_cast<uint8>(left) ^ static_cast<uint8>(right)));
    }

    // bitwise NOT, every static_cast is intentional
    REX_NO_DISCARD constexpr byte operator~(const byte arg) noexcept
    {
      return static_cast<byte>(static_cast<uint8>(~static_cast<uint8>(arg)));
    }

    // bitwise LEFT SHIFT
    template <typename IntType, enable_if_t<is_integral_v<IntType>, bool> = true>
    constexpr byte& operator<<=(byte& arg, const IntType shift) noexcept
    {
      return arg = arg << shift;
    }

    // bitwise RIGHT SHIFT
    template <typename IntType, enable_if_t<is_integral_v<IntType>, bool> = true>
    constexpr byte& operator>>=(byte& arg, const IntType shift) noexcept
    {
      return arg = arg >> shift;
    }

    // bitwise OR
    constexpr byte& operator|=(byte& left, const byte right) noexcept
    {
      return left = left | right;
    }

    // bitwise AND
    constexpr byte& operator&=(byte& left, const byte right) noexcept
    {
      return left = left & right;
    }

    // bitwise XOR
    constexpr byte& operator^=(byte& left, const byte right) noexcept
    {
      return left = left ^ right;
    }

    // convert byte to integer
    template <typename IntType, enable_if_t<is_integral_v<IntType>, bool> = true>
    REX_NO_DISCARD constexpr IntType to_integer(const byte arg) noexcept
    {
      return static_cast<IntType>(arg);
    }

  } // namespace v1
} // namespace rsl
