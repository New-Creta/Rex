// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: signbit.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#include "rex_std/bonus/types.h"
#include "rex_std/internal/type_traits/is_integral.h"
#include "rex_std/internal/type_traits/is_unsigned.h"

namespace rsl
{
  inline namespace v1
  {

    constexpr bool signbit(int8 arg)
    {
      return arg & 0x80;
    }
    constexpr bool signbit(int16 arg)
    {
      return arg & 0x8000;
    }
    constexpr bool signbit(int32 arg)
    {
      return arg & 0x80000000;
    }
    constexpr bool signbit(int64 arg)
    {
      return arg & 0x8000000000000000;
    }
    template <typename Unsigned>
    constexpr bool signbit(Unsigned arg)
    {
      static_assert(rsl::is_integral_v<Unsigned>, "arg must be an integral type");
      static_assert(rsl::is_unsigned_v<Unsigned>, "arg must be unsigned");

      return false;
    }
    constexpr bool signbit(float32 arg)
    {
      // type punning is undefined behavior
      int32 arg_int = static_cast<int32>(arg);
      return signbit(arg_int);
    }
    constexpr bool signbit(float64 arg)
    {
      // type punning is undefined behavior
      int64 arg_int = static_cast<int64>(arg);
      return signbit(arg_int);
    }
    constexpr bool signbit(lfloat64 arg)
    {
      // type punning is undefined behavior
      int64 arg_int = static_cast<int64>(arg);
      return signbit(arg_int);
    }

  } // namespace v1
} // namespace rsl
