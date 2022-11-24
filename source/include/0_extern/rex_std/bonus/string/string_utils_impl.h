// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: string_utils_impl.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/string/character_lookup.h"
#include "rex_std/bonus/types.h"
#include "rex_std/internal/algorithm/reverse.h"
#include "rex_std/internal/iterator/distance.h"
#include "rex_std/internal/iterator/end.h"
#include "rex_std/internal/iterator/random_access_iterator.h"
#include "rex_std/internal/iterator/reverse_iterator.h"
#include "rex_std/internal/type_traits/is_integral.h"
#include "rex_std/internal/type_traits/is_unsigned.h"
#include "rex_std/internal/type_traits/make_unsigned.h"
#include "rex_std/iterator.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      constexpr card32 max_int_buffer = 21;

      template <typename CharType, typename T>
      constexpr CharType* unsigned_integral_to_buff(CharType* rnext, T value)
      {
        static_assert(rsl::is_unsigned_v<T>, "T must be unsigned");

        auto value_trunc = value;

        do
        {
          *--rnext = static_cast<T>(static_cast<CharType>('0') + value_trunc % 10);
          value_trunc /= 10;
        } while(value_trunc != 0);
        return rnext;
      }

      template <typename StringType, typename T>
      constexpr StringType signed_to_string(T value)
      {
        static_assert(rsl::is_integral_v<T>, "T must be integral");

        using char_type = typename StringType::value_type;

        char_type buff[max_int_buffer]; // NOLINT(modernize-avoid-c-arrays) // can hold -2^63 and 2^64 - 1, plus NULL
        char_type* buff_end       = rsl::end(buff);
        char_type* rnext          = buff_end;
        const auto unsigned_value = static_cast<rsl::make_unsigned_t<T>>(value);

        if(value < 0)
        {
          rnext = unsigned_integral_to_buff(rnext, 0 - unsigned_value);
        }
        else
        {
          rnext = unsigned_integral_to_buff(rnext, unsigned_value);
        }

        const size_t size = buff_end - rnext;
        return StringType(rnext, static_cast<count_t>(size));
      }

      template <typename StringType, typename T>
      constexpr StringType unsigned_to_string(T value)
      {
        static_assert(rsl::is_integral_v<T>, "T must be integral");
        static_assert(rsl::is_unsigned_v<T>, "T must be unsigned");

        using char_type = typename StringType::value_type;

        char_type buff[max_int_buffer]; // NOLINT(modernize-avoid-c-arrays) // can hold -2^63 and 2^64 - 1, plus NULL
        char_type* buff_end = rsl::end(buff);
        char_type* rnext    = unsigned_integral_to_buff(buff_end, value);

        const size_t size = buff_end - rnext;
        return StringType(rnext, static_cast<count_t>(size));
      }
    } // namespace internal

    template <typename SizeType, typename Iterator>
    constexpr SizeType find(Iterator srcBegin, Iterator srcEnd, Iterator toFindBegin, SizeType toFindLength, SizeType defaultValue)
    {
      Iterator to_find_str_end = toFindBegin + toFindLength;
      auto it                  = search(srcBegin, srcEnd, toFindBegin, to_find_str_end);
      return it != srcEnd ? rsl::distance(srcBegin, it) : defaultValue;
    }
    template <typename SizeType, typename Iterator>
    constexpr SizeType find(Iterator srcBegin, Iterator srcEnd, Iterator toFindBegin, SizeType defaultValue)
    {
      SizeType to_find_length = string_length(toFindBegin);
      return find<SizeType>(srcBegin, srcEnd, toFindBegin, to_find_length, defaultValue);
    }
    template <typename SizeType, typename Iterator>
    constexpr SizeType rfind(Iterator srcBegin, Iterator srcEnd, Iterator toFindBegin, SizeType toFindLength, SizeType defaultValue)
    {
      Iterator to_find_str_end = toFindBegin + toFindLength;
      auto it                  = search(srcBegin, srcEnd, toFindBegin, to_find_str_end);
      return it != srcEnd ? rsl::distance(srcBegin, it) : defaultValue;
    }
    template <typename SizeType, typename Iterator>
    constexpr SizeType rfind(Iterator srcBegin, Iterator srcEnd, Iterator toFindBegin, SizeType defaultValue)
    {
      SizeType to_find_length = string_length(toFindBegin);
      return rfind<SizeType>(srcBegin, srcEnd, toFindBegin, to_find_length, defaultValue);
    }
    template <typename SizeType, typename Iterator>
    constexpr SizeType find_first_of(Iterator srcBegin, SizeType numCharsToCheck, Iterator toFindBegin, SizeType toFindLength, SizeType defaultValue)
    {
      character_lookup<typename Iterator::value_type> lookup(iterator_to_pointer(toFindBegin), toFindLength);

      for(SizeType i = 0; i < numCharsToCheck; ++i)
      {
        auto c = srcBegin[i];
        if(lookup.exists(c))
        {
          return i;
        }
      }

      return defaultValue;
    }
    template <typename SizeType, typename Iterator>
    constexpr SizeType find_first_of(Iterator srcBegin, SizeType numCharsToCheck, Iterator toFindBegin, SizeType defaultValue)
    {
      SizeType to_find_length = string_length(toFindBegin);
      return find_first_of<SizeType>(srcBegin, numCharsToCheck, toFindBegin, to_find_length, defaultValue);
    }
    template <typename SizeType, typename Iterator>
    constexpr SizeType find_first_not_of(Iterator srcBegin, SizeType numCharsToCheck, Iterator toFindBegin, SizeType toFindLength, SizeType defaultValue)
    {
      character_lookup<typename Iterator::value_type> lookup(iterator_to_pointer(toFindBegin), toFindLength);

      for(SizeType i = 0; i < numCharsToCheck; ++i)
      {
        auto c = srcBegin[i];
        if(!lookup.exists(c))
        {
          return i;
        }
      }

      return defaultValue;
    }
    template <typename SizeType, typename Iterator>
    constexpr SizeType find_first_not_of(Iterator srcBegin, SizeType numCharsToCheck, Iterator toFindBegin, SizeType defaultValue)
    {
      SizeType to_find_length = string_length(toFindBegin);
      return find_first_not_of<SizeType>(srcBegin, numCharsToCheck, toFindBegin, to_find_length, defaultValue);
    }
    template <typename SizeType, typename Iterator>
    constexpr SizeType find_last_of(Iterator srcBegin, SizeType numCharsToCheck, Iterator toFindBegin, SizeType toFindLength, SizeType defaultValue)
    {
      character_lookup<typename Iterator::value_type> lookup(iterator_to_pointer(toFindBegin), toFindLength);

      for(SizeType i = numCharsToCheck; i >= 0; --i)
      {
        auto c = srcBegin[i];
        if(lookup.exists(c))
        {
          return i;
        }
      }

      return defaultValue;
    }
    template <typename SizeType, typename Iterator>
    constexpr SizeType find_last_of(Iterator srcBegin, SizeType numCharsToCheck, Iterator toFindBegin, SizeType defaultValue)
    {
      SizeType to_find_length = string_length(toFindBegin);
      return find_last_of<SizeType>(srcBegin, numCharsToCheck, toFindBegin, to_find_length, defaultValue);
    }
    template <typename SizeType, typename Iterator>
    constexpr SizeType find_last_not_of(Iterator srcBegin, SizeType numCharsToCheck, Iterator toFindBegin, SizeType toFindLength, SizeType defaultValue)
    {
      character_lookup<typename Iterator::value_type> lookup(iterator_to_pointer(toFindBegin), toFindLength);

      for(SizeType i = numCharsToCheck; i >= 0; --i)
      {
        auto c = srcBegin[i];
        if(!lookup.exists(c))
        {
          return i;
        }
      }

      return defaultValue;
    }
    template <typename SizeType, typename Iterator>
    constexpr SizeType find_last_not_of(Iterator srcBegin, SizeType numCharsToCheck, Iterator toFindBegin, SizeType defaultValue)
    {
      SizeType to_find_length = string_length(toFindBegin);
      return find_last_not_of<SizeType>(srcBegin, numCharsToCheck, toFindBegin, to_find_length, defaultValue);
    }

    namespace internal
    {
      template <typename StringType>
      constexpr StringType to_string(const int32 value)
      {
        return internal::signed_to_string<StringType>(value);
      }
      template <typename StringType>
      constexpr StringType to_string(const long value)
      {
        return internal::signed_to_string<StringType>(static_cast<int32>(value));
      }
      template <typename StringType>
      constexpr StringType to_string(const uint32 value)
      {
        return internal::unsigned_to_string<StringType>(value);
      }
      template <typename StringType>
      constexpr StringType to_string(const ulong value)
      {
        return internal::unsigned_to_string<StringType>(static_cast<uint32>(value));
      }
      template <typename StringType>
      constexpr StringType to_string(const int64 value)
      {
        return internal::signed_to_string<StringType>(value);
      }
      template <typename StringType>
      constexpr StringType to_string(const uint64 value)
      {
        return internal::unsigned_to_string<StringType>(value);
      }
      template <typename StringType>
      constexpr StringType to_string(const float32 value, card32 precision)
      {
        const card32 int_value = static_cast<card32>(value);

        const float32 radix_value        = value - int_value;                     // NOLINT(cppcoreguidelines-narrowing-conversions)
        const float32 radix_value_to_int = radix_value * rsl::pow(10, precision); // NOLINT(cppcoreguidelines-narrowing-conversions)
        const card32 radix_value_as_int  = static_cast<card32>(radix_value_to_int);

        return to_string<StringType>(int_value) + static_cast<typename StringType::value_type>('.') + to_string<StringType>(radix_value_as_int);
      }
      template <typename StringType>
      constexpr StringType to_string(const float64 value, card32 precision)
      {
        const card64 int_value = static_cast<card64>(value);

        const float64 radix_value        = value - int_value; // NOLINT(cppcoreguidelines-narrowing-conversions)
        const float64 radix_value_to_int = radix_value * rsl::pow(10, precision);
        const card64 radix_value_as_int  = static_cast<card64>(radix_value_to_int);

        return to_string<StringType>(int_value) + static_cast<typename StringType::value_type>('.') + to_string<StringType>(radix_value_as_int);
      }
      template <typename StringType>
      constexpr StringType to_string(const lfloat64 value, card32 precision)
      {
        return to_string<StringType>(static_cast<const float64>(value), precision);
      }
      template <typename StringType>
      constexpr StringType to_string(const void* ptr)
      {
        uintptr ptr_as_int = reinterpret_cast<uintptr>(ptr); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)

        StringType str;
        str.resize(max_int_buffer + 2);
        auto it = str.rbegin();
        *it     = '0';
        ++it;
        *it = 'x';
        ++it;

        constexpr char8 digits[] = "0123456789abcdef"; // NOLINT(modernize-avoid-c-arrays)

        do
        {
          *it = digits[ptr_as_int & 15u];
          ptr_as_int >>= 4; // divide by 16
          ++it;
        } while(ptr_as_int);

        rsl::reverse(str.begin(), str.end());
        return str;
      }
    } // namespace internal

  } // namespace v1
} // namespace rsl
