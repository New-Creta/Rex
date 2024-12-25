#pragma once

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/engine/types.h"
#include "rex_std/type_traits.h"

#ifndef REX_BUILD_RELEASE
  #define REX_ENABLE_SAFE_CASTING
#endif

namespace rex
{
  // narrow casting
  // casts a value to a type that has less bits than the original one
  // the safe version of narrow cast performs a static cast
  // and checks if any value is lost after the cast
  template <typename NarrowType, typename T>
  constexpr NarrowType unsafe_narrow_cast(const T value)
  {
    static_assert(rsl::is_arithmetic_v<T>, "T must be arithmetic type!");
    return static_cast<NarrowType>(value);
  }

  template <typename NarrowType, typename T>
  constexpr NarrowType safe_narrow_cast(const T value)
  {
    static_assert(rsl::is_arithmetic_v<T>, "T must be arithmetic type!");

    NarrowType narrow_val = static_cast<NarrowType>(value);
    if(narrow_val != value)
    {
      REX_ERROR(LogEngine, "Narrow cast loses value");
    }

    return narrow_val;
  }

  template <typename NarrowType, typename T>
  constexpr NarrowType narrow_cast(const T value)
  {
    static_assert(rsl::is_arithmetic_v<T>, "T must be arithmetic type!");

#ifdef REX_ENABLE_SAFE_CASTING
    return safe_narrow_cast<NarrowType>(value);
#else
    return unsafe_narrow_cast<NarrowType>(value);
#endif
  }

  // sign casting
  // casts a value to a type by only changing the sign. It's not allowed to reduce the size of the type
  // the safe version of sign cast performs a static cast
  // and checks if any value is lost after the cast
  template <typename OtherSignType, typename T>
  constexpr OtherSignType unsafe_sign_cast(const T value)
  {
    static_assert(rsl::is_integral_v<T>, "T must be of integral type!");
    static_assert(sizeof(T) == sizeof(OtherSignType), "sign casting between different sizes");

    return static_cast<OtherSignType>(value);
  }

  template <typename OtherSignType, typename T>
  constexpr OtherSignType safe_sign_cast(const T value)
  {
    static_assert(rsl::is_integral_v<T>, "T must be of integral type!");
    static_assert(sizeof(T) == sizeof(OtherSignType), "sign casting between different sizes");

    OtherSignType sign_val        = static_cast<OtherSignType>(value);
    constexpr auto max_signed_val = (rsl::numeric_limits<rsl::make_signed_t<T>>::max)();

    // It's okay to disable the warning here as we'd get a compiler error anyway,
    // should this function be called at compile time
#pragma warning(push)
#pragma warning(disable : 4389)
    // comparing the values here would never trigger because the signed would be promoted to an unsigned
    if(sign_val > max_signed_val || sign_val < 0)
    {
      REX_ERROR(LogEngine, "Sign cast loses value");
    }
#pragma warning(pop)

    return sign_val;
  }

  template <typename OtherSignType, typename T>
  constexpr OtherSignType sign_cast(const T value)
  {
#ifdef REX_ENABLE_SAFE_CASTING
    return safe_sign_cast<OtherSignType>(value);
#else
    return unsafe_sign_cast<OtherSignType>(value);
#endif
  }

  // flipping signs
  // simply casts a value to a type of the same size but with a different sign
  // the safe version of this function perform a static cast
  // and checks if any value is lost after the cast
  template <typename T>
  constexpr auto unsafe_flip_sign(const T value)
  {
    if constexpr(rsl::is_signed_v<T>)
    {
      using other_sign_type = rsl::make_unsigned_t<T>;
      return unsafe_sign_cast<other_sign_type>(value);
    }
    else
    {
      using other_sign_type = rsl::make_signed_t<T>;
      return unsafe_sign_cast<other_sign_type>(value);
    }
  }

  template <typename T>
  constexpr auto safe_flip_sign(const T value)
  {
    if constexpr(rsl::is_signed_v<T>)
    {
      using other_sign_type = rsl::make_unsigned_t<T>;
      return safe_sign_cast<other_sign_type>(value);
    }
    else
    {
      using other_sign_type = rsl::make_signed_t<T>;
      return safe_sign_cast<other_sign_type>(value);
    }
  }

  template <typename T>
  constexpr auto flip_sign(const T value)
  {
#ifdef REX_ENABLE_SAFE_CASTING
    return safe_flip_sign(value);
#else
    return unsafe_flip_sign(value);
#endif
  }

  // numeric casting
  // numeric casting is combination of both sign casting and narrowing
  // the resulting type can be a different sign and smaller in size than the original
  // the safe version of this function performs a static cast
  // and checks if any value is lost after the cast
  template <typename OtherType, typename T>
  constexpr OtherType unsafe_numeric_cast(const T value)
  {
    if constexpr(rsl::is_signed_v<T>)
    {
      using OtherSignedType              = rsl::make_unsigned_t<T>;
      OtherSignedType other_signed_value = sign_cast<OtherSignedType>(value);
      return unsafe_narrow_cast<OtherType>(other_signed_value);
    }
    else
    {
      using OtherSignedType              = rsl::make_signed_t<T>;
      OtherSignedType other_signed_value = sign_cast<OtherSignedType>(value);
      return unsafe_narrow_cast<OtherType>(other_signed_value);
    }
  }

  template <typename OtherType, typename T>
  constexpr OtherType safe_numeric_cast(const T value)
  {
    if constexpr(rsl::is_signed_v<T>)
    {
      using OtherSignedType              = rsl::make_unsigned_t<T>;
      OtherSignedType other_signed_value = safe_sign_cast<OtherSignedType>(value);
      return safe_narrow_cast<OtherType>(other_signed_value);
    }
    else
    {
      using OtherSignedType              = rsl::make_signed_t<T>;
      OtherSignedType other_signed_value = safe_sign_cast<OtherSignedType>(value);
      return safe_narrow_cast<OtherType>(other_signed_value);
    }
  }

  template <typename OtherType, typename T>
  constexpr OtherType numeric_cast(const T value)
  {
#ifdef REX_ENABLE_SAFE_CASTING
    return safe_numeric_cast<OtherType>(value);
#else
    return unsafe_numeric_cast<OtherType>(value);
#endif
  }

  // Utility function to cast a pointer into a byte array
  template <typename T>
  constexpr const rsl::byte* byte_cast(const T* data)
  {
    return reinterpret_cast<const rsl::byte*>(data); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
  }
  // Utility function to cast a pointer into a byte array
  template <typename T>
  constexpr rsl::byte* byte_cast(T* data)
  {
    return reinterpret_cast<rsl::byte*>(data); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
  }

  // Utility function to cast a pointer into a char array
  template <typename T>
  constexpr const char8* char_cast(const T* data)
  {
    return reinterpret_cast<const char8*>(data); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
  }
  // Utility function to cast a pointer into a char array
  template <typename T>
  constexpr char8* char_cast(T* data)
  {
    return reinterpret_cast<char8*>(data); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
  }

  // converts an enum to an int
  template <typename E, rsl::enable_if_t<rsl::is_enum_v<E>, bool> = true>
  constexpr s32 to_int(E e)
  {
    return static_cast<s32>(e);
  }
} // namespace rex