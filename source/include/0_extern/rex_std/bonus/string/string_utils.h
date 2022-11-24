// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: string_utils.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/attributes.h"
#include "rex_std/bonus/string/character_lookup.h"
#include "rex_std/bonus/type_traits/is_character.h"
#include "rex_std/bonus/types.h"
#include "rex_std/ctype.h"
#include "rex_std/internal/algorithm/max.h"
#include "rex_std/internal/algorithm/memcmp.h"
#include "rex_std/internal/algorithm/min.h"
#include "rex_std/internal/math/pow.h"
#include "rex_std/internal/memory/memcpy.h"
#include "rex_std/internal/string/char_traits.h"
#include "rex_std/internal/type_traits/is_unsigned.h"
#include "rex_std/internal/utility/size.h"
#include "rex_std/limits.h"
#include "rex_std/optional.h"

namespace rsl
{
  inline namespace v1
  {

    //
    // Character Classification (cctype.h)
    //

    //
    // Character Manipulation
    //

    //// converts a character to lowercase
    // REX_NO_DISCARD constexpr char8 to_lower(char8 ch);
    //// converts a character to uppercase
    // REX_NO_DISCARD constexpr char8 to_upper(char8 ch);

    //
    // Conversion To Numeric Formats (cstdlib.h)
    //

    REX_NO_DISCARD constexpr tchar to_wide_char(char8 chr)
    {
      return static_cast<tchar>(chr);
    }
    REX_NO_DISCARD constexpr char8 to_ascii(tchar chr)
    {
      return static_cast<char8>(chr);
    }

    template <typename Iterator>
    REX_NO_DISCARD constexpr bool is_letter(Iterator letter)
    {
      return is_upper(letter) || is_lower(letter);
    }

    template <typename Iterator>
    REX_NO_DISCARD constexpr bool is_sign(Iterator sign)
    {
      return sign == '-' || sign == '+';
    }

    REX_NO_DISCARD constexpr bool is_multibyte(char8 c)
    {
      return (c & 0x80) != 0; // NOLINT(readability-magic-numbers, hicpp-signed-bitwise)
    }

    template <typename Iterator>
    REX_NO_DISCARD constexpr card32 ctoi(Iterator digit)
    {
      return digit - '0';
    }

    template <typename Iterator>
    REX_NO_DISCARD constexpr card32 string_length(const Iterator* str)
    {
      static_assert(is_character_v<Iterator>, "argument is not of character type");

      card32 length = 0;
      while(*str)
      {
        str++;
        ++length;
      }
      return length;
    }

    template <typename Iterator>
    REX_NO_DISCARD constexpr bool string_compare(const Iterator* str, card32 length, bool (*stringCompareFunc)(Iterator))
    {
      for(card32 i = 0; i < length; ++i)
      {
        if(!stringCompareFunc(str[i]))
          return false;
      }

      return true;
    }

    template <typename Iterator>
    REX_NO_DISCARD constexpr bool string_equals(const Iterator* lhs, card32 lhsLength, const Iterator* rhs, card32 rhsLength)
    {
      if(lhsLength != rhsLength)
        return false;

      for(card32 i = 0; i < lhsLength; ++i)
      {
        if(lhs[i] != rhs[i])
          return false;
      }

      return true;
    }

    template <typename Iterator>
    REX_NO_DISCARD constexpr bool string_equals(const Iterator* lhs, const Iterator* rhs, card32 rhsLength)
    {
      for(card32 i = 0; i < rhsLength; ++i)
      {
        if(lhs[i] != rhs[i])
          return false;
      }

      return true;
    }

    template <typename Iterator>
    REX_NO_DISCARD constexpr bool string_equals_case_insensitive(const char* lhs, card32 lhsLength, const Iterator* rhs, card32 rhsLength)
    {
      if(lhsLength != rhsLength)
        return false;

      for(card32 i = 0; i < lhsLength; ++i)
      {
        Iterator lhs_c = to_lower(lhs[i]);
        Iterator rhs_c = to_lower(rhs[i]);

        if(lhs_c != rhs_c)
        {
          return false;
        }
      }

      return true;
    }
    template <typename Iterator>
    REX_NO_DISCARD constexpr int32 string_lex_compare(const char* lhs, const char* rhs, card32 count)
    {
      return rsl::memcmp(lhs, rhs, count);
    }

    template <typename Iterator>
    REX_NO_DISCARD constexpr bool ends_with(const Iterator* str, card32 strLength, const Iterator* suffix, card32 suffixLength)
    {
      if(strLength < suffixLength)
      {
        return false;
      }

      const char* start = str + (strLength - suffixLength);

      return string_equals(start, suffixLength, suffix, suffixLength);
    }
    template <typename Iterator>
    REX_NO_DISCARD constexpr bool ends_with(const Iterator* str, const Iterator* suffix)
    {
      card32 str_length    = string_length(str);
      card32 suffix_length = string_length(suffix);

      return ends_with(str, str_length, suffix, suffix_length);
    }

    template <typename Iterator>
    REX_NO_DISCARD constexpr bool starts_with(const Iterator* str, card32 strLength, const Iterator* prefix, card32 prefixLength)
    {
      if(strLength < prefixLength)
      {
        return false;
      }

      return string_equals(str, prefixLength, prefix, prefixLength);
    }
    template <typename Iterator>
    REX_NO_DISCARD constexpr bool starts_with(const Iterator* str, const Iterator* prefix)
    {
      card32 str_length    = string_length(str);
      card32 prefix_length = string_length(prefix);

      return starts_with(str, str_length, prefix, prefix_length);
    }

    template <typename Iterator>
    REX_NO_DISCARD constexpr bool is_letters(const Iterator* letters, card32 length)
    {
      return string_compare(letters, length, is_letter);
    }
    template <typename Iterator>
    REX_NO_DISCARD constexpr bool is_letters(const Iterator* letters)
    {
      card32 length = string_length(letters);
      return is_letters(letters, length);
    }
    template <typename Iterator>
    REX_NO_DISCARD constexpr bool is_digits(const Iterator* digits, card32 length)
    {
      return (is_sign(*digits) && string_compare(digits + 1, length - 1, is_digit)) || string_compare(digits, length, is_digit);
    }
    template <typename Iterator>
    REX_NO_DISCARD constexpr bool is_digits(const Iterator* digits)
    {
      card32 length = string_length(digits);
      return (is_sign(*digits) && string_compare(digits + 1, length - 1, is_digit)) || string_compare(digits, length, is_digit);
    }
    template <typename Iterator>
    REX_NO_DISCARD constexpr bool is_digitsf(const Iterator* digits, card32 length)
    {
      return (is_sign(*digits) && string_compare(digits + 1, length - 1, is_digitf)) || string_compare(digits, length, is_digitf);
    }
    template <typename Iterator>
    REX_NO_DISCARD constexpr bool is_digitsf(const Iterator* digits)
    {
      card32 length = string_length(digits);
      return (is_sign(*digits) && string_compare(digits + 1, length - 1, is_digitf)) || string_compare(digits, length, is_digitf);
    }
    template <typename Iterator>
    REX_NO_DISCARD constexpr bool is_nan(const Iterator* digits, card32 length)
    {
      return !is_digits(digits, length);
    }
    template <typename Iterator>
    REX_NO_DISCARD constexpr bool is_nan(const Iterator* digits)
    {
      card32 length = string_length(digits);
      return is_nan(digits, length);
    }
    template <typename Iterator>
    REX_NO_DISCARD constexpr bool is_nanf(const Iterator* digits, card32 length)
    {
      return !is_digitsf(digits, length);
    }
    template <typename Iterator>
    REX_NO_DISCARD constexpr bool is_nanf(const Iterator* digits)
    {
      card32 length = string_length(digits);
      return !is_digitsf(digits, length);
    }

    template <typename Iterator>
    REX_NO_DISCARD constexpr bool is_lower(const Iterator* letters, card32 length)
    {
      return string_compare(letters, length, is_lower);
    }
    template <typename Iterator>
    REX_NO_DISCARD constexpr bool is_lower(const Iterator* letters)
    {
      card32 length = string_length(letters);
      return string_compare(letters, length, is_lower);
    }
    template <typename Iterator>
    REX_NO_DISCARD constexpr bool is_upper(const Iterator* letters, card32 length)
    {
      return string_compare(letters, length, is_upper);
    }
    template <typename Iterator>
    REX_NO_DISCARD constexpr bool is_upper(const Iterator* letters)
    {
      card32 length = string_length(letters);
      return string_compare(letters, length, is_upper);
    }

    template <typename Iterator>
    constexpr void to_lower(const Iterator* str, Iterator* buf, card32 length)
    {
      for(card32 i = 0; i < length; ++i)
      {
        buf[i] = to_lower(str[i]);
      }
    }

    template <typename Iterator, card32 Size>
    constexpr void to_lower(const Iterator* str, Iterator (&buf)[Size]) // NOLINT(modernize-avoid-c-arrays)
    {
      to_lower(str, buf, Size - 1);
    }
    template <typename Iterator>
    constexpr void to_upper(const Iterator* str, Iterator* buf, card32 length)
    {
      for(card32 i = 0; i < length; ++i)
      {
        buf[i] = to_upper(str[i]);
      }
    }

    template <typename Iterator, card32 Size>
    constexpr void to_upper(const Iterator* str, Iterator (&buf)[Size]) // NOLINT(modernize-avoid-c-arrays)
    {
      to_upper(str, buf, Size - 1);
    }

    template <typename Iterator>
    REX_NO_DISCARD constexpr optional<float32> stof(const Iterator* str, card32 length)
    {
      float32 before_radix_value = 0.0f;
      float32 after_radix_value  = 0.0f;

      int32 sign                  = 1;
      card32 i                    = 0;
      card32 digits_after_radix   = 0;
      bool assigning_before_radix = true;

      if(str[0] == '-')
      {
        sign = -1;
        ++i;
      }
      else if(str[0] == '+')
      {
        ++i;
      }

      for(; i < length; ++i)
      {
        if(!is_digitf(str[i]))
          return nullopt;

        if(str[i] == '.')
        {
          assigning_before_radix = false;
          continue;
        }

        if(assigning_before_radix)
        {
          before_radix_value = ctoi(str[i]) + before_radix_value * 10.0f; // NOLINT(readability-magic-numbers)
        }
        else
        {
          after_radix_value = ctoi(str[i]) + after_radix_value * 10.0f; // NOLINT(readability-magic-numbers)
          ++digits_after_radix;
        }
      }

      return sign * before_radix_value + (after_radix_value / (max(1.0f, pow(10.0f, digits_after_radix)))); // NOLINT(readability-magic-numbers, bugprone-narrowing-conversions, cppcoreguidelines-narrowing-conversions)
    }
    template <typename Iterator>
    REX_NO_DISCARD constexpr optional<int32> stoi(const Iterator* str, card32 length)
    {
      int32 value = 0;
      int32 sign  = 1;
      int32 i     = 0;

      if(str[0] == '-')
      {
        sign = -1;
        ++i;
      }
      else if(str[0] == '+')
      {
        ++i;
      }

      for(; i < length; ++i)
      {
        if(!is_digit(str[i]))
          return nullopt;

        value = ctoi(str[i]) + value * 10; // NOLINT(readability-magic-numbers)
      }

      return sign * value;
    }
    template <typename Iterator>
    REX_NO_DISCARD constexpr optional<uint32> stoui(const Iterator* str, card32 length)
    {
      uint32 value = 0;

      for(card32 i = 0; i < length; ++i)
      {
        if(!is_digit(str[i]))
          return nullopt;

        value = ctoi(str[i]) + value * 10; // NOLINT(readability-magic-numbers)
      }

      return value;
    }
    template <typename Iterator>
    REX_NO_DISCARD constexpr optional<bool> stob(const Iterator* str, card32 length)
    {
      constexpr Iterator true_str[]  = "true";  // NOLINT(modernize-avoid-c-arrays)
      constexpr Iterator false_str[] = "false"; // NOLINT(modernize-avoid-c-arrays)

      if(string_equals(str, length, true_str, size(true_str)))
      {
        return true;
      }
      else if(string_equals(str, length, false_str, size(false_str)))
      {
        return false;
      }

      return stoi(str, length);
    }

    template <typename SizeType, typename Iterator>
    constexpr SizeType find(Iterator srcBegin, Iterator srcEnd, Iterator toFindBegin, SizeType toFindLength, SizeType defaultValue);
    template <typename SizeType, typename Iterator>
    constexpr SizeType find(Iterator srcBegin, Iterator srcEnd, Iterator toFindBegin, SizeType defaultValue);
    template <typename SizeType, typename Iterator>
    constexpr SizeType rfind(Iterator srcBegin, Iterator srcEnd, Iterator toFindBegin, SizeType toFindLength, SizeType defaultValue);
    template <typename SizeType, typename Iterator>
    constexpr SizeType rfind(Iterator srcBegin, Iterator srcEnd, Iterator toFindBegin, SizeType defaultValue);
    template <typename SizeType, typename Iterator>
    constexpr SizeType find_first_of(Iterator srcBegin, SizeType numCharsToCheck, Iterator toFindBegin, SizeType toFindLength, SizeType defaultValue);
    template <typename SizeType, typename Iterator>
    constexpr SizeType find_first_of(Iterator srcBegin, SizeType numCharsToCheck, Iterator toFindBegin, SizeType defaultValue);
    template <typename SizeType, typename Iterator>
    constexpr SizeType find_first_not_of(Iterator srcBegin, SizeType numCharsToCheck, Iterator toFindBegin, SizeType toFindLength, SizeType defaultValue);
    template <typename SizeType, typename Iterator>
    constexpr SizeType find_first_not_of(Iterator srcBegin, SizeType numCharsToCheck, Iterator toFindBegin, SizeType defaultValue);
    template <typename SizeType, typename Iterator>
    constexpr SizeType find_last_of(Iterator srcBegin, SizeType numCharsToCheck, Iterator toFindBegin, SizeType toFindLength, SizeType defaultValue);
    template <typename SizeType, typename Iterator>
    constexpr SizeType find_last_of(Iterator srcBegin, SizeType numCharsToCheck, Iterator toFindBegin, SizeType defaultValue);
    template <typename SizeType, typename Iterator>
    constexpr SizeType find_last_not_of(Iterator srcBegin, SizeType numCharsToCheck, Iterator toFindBegin, SizeType toFindLength, SizeType defaultValue);
    template <typename SizeType, typename Iterator>
    constexpr SizeType find_last_not_of(Iterator srcBegin, SizeType numCharsToCheck, Iterator toFindBegin, SizeType defaultValue);

    namespace internal
    {
      template <typename CharType, typename T>
      constexpr CharType* unsigned_integral_to_buff(CharType* rnext, T value);
      template <typename StringType, typename T>
      constexpr StringType signed_to_string(T value);
      template <typename StringType, typename T>
      constexpr StringType unsigned_to_string(T value);

      template <typename StringType>
      constexpr StringType to_string(int32 value);
      template <typename StringType>
      constexpr StringType to_string(uint32 value);
      template <typename StringType>
      constexpr StringType to_string(int64 value);
      template <typename StringType>
      constexpr StringType to_string(uint64 value);
      template <typename StringType>
      constexpr StringType to_string(float32 value, card32 precision = 4);
      template <typename StringType>
      constexpr StringType to_string(float64 value, card32 precision = 4);
      template <typename StringType>
      constexpr StringType to_string(lfloat64 value, card32 precision = 4);
      template <typename StringType>
      constexpr StringType to_string(void* ptr);

      // iterator can be of type:
      // - char8*
      // - tchar*
      // - istreambuf_iterator
      // - random_access_iterator
      template <typename T, typename Iterator, typename IteratorPointer>
      constexpr optional<T> str_to_signed(Iterator str, IteratorPointer strEnd, int32 base)
      {
        static_assert(rsl::is_signed_v<T>, "T must be a signed type");

        // skip white space
        while(is_space(*str))
        {
          ++str;
        }

        // determine base
        if((base == 0 || base == 16) && *str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X')) // NOLINT(readability-magic-numbers)
        {
          str += 2;
          return 16; // NOLINT(readability-magic-numbers)
        }
        if(base == 0)
        {
          base = *str == '0' ? 8 : 10; // NOLINT(readability-magic-numbers)
        }

        // determine sign
        int32 sign = 1;
        if(*str == '-')
        {
          sign = -1;
          ++str;
        }
        else if(*str == '+')
        {
          ++str;
        }

        // process string
        T value        = 0;
        bool value_set = false;

        while(*str != '\0')
        {
          auto c = *str;

          if(is_digit(c))
          {
            c -= '0';
          }
          else if(is_alpha(c))
          { /* is_alpha(c))*/
            c -= is_upper(c) ? 'A' - 10 : 'a' - 10;
          }
          else
          {
            break;
          }

          if(c >= base)
          {
            break;
          }
          else
          {
            value_set = true;
            value *= base;
            value += c;
          }

          ++str;
        }

        // return result
        if(strEnd)
        {
          /**str_end = const_cast<Iterator>(str);*/
        }

        if(value_set)
        {
          return optional<T>(value * sign);
        }
        else
        {
          return nullopt;
        }
      }

      template <typename T, typename Iterator>
      constexpr optional<T> str_to_signed(const Iterator str, int32 base)
      {
        return str_to_signed<T>(str, static_cast<Iterator*>(nullptr), base);
      }

      // iterator can be of type:
      // - char8*
      // - tchar*
      // - istreambuf_iterator
      // - random_access_iterator
      template <typename T, typename Iterator, typename IteratorPointer>
      constexpr optional<T> str_to_unsigned(Iterator str, IteratorPointer /*str_end*/, int32 base)
      {
        static_assert(rsl::is_unsigned_v<T>, "T must be a unsigned type");

        // skip white space
        while(is_space(*str))
        {
          ++str;
        }

        // determine base
        if((base == 0 || base == 16) && *str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X'))
        {
          str += 2;
          return 16;
        }
        if(base == 0)
        {
          base = *str == '0' ? 8 : 10;
        }

        // process string
        T value        = 0;
        bool value_set = false;
        while(*str != '\0')
        {
          auto c = *str;

          if(is_digit(c))
          {
            c -= '0';
          }
          else if(is_alpha(c))
          {
            c -= is_upper(c) ? 'A' - 10 : 'a' - 10;
          }
          else
          {
            break;
          }

          if(c >= base)
          {
            break;
          }
          else
          {
            value_set = true;
            value *= base;
            value += c;
          }

          ++str;
        }

        // return result
        // if (str_end)
        //{
        //  *str_end = const_cast<Iterator>(str);
        //}
        if(value_set)
        {
          return optional<T>(value);
        }
        else
        {
          return nullopt;
        }
      }

      template <typename T, typename Iterator>
      constexpr optional<T> str_to_unsigned(Iterator str, int32 base)
      {
        return str_to_unsigned<T>(str, static_cast<Iterator*>(nullptr), base);
      }
      // iterator can be of type:
      // - char8*
      // - tchar*
      // - istreambuf_iterator
      // - random_access_iterator
      template <typename T, typename Iterator, typename IteratorPointer>
      constexpr optional<T> str_to_floating_point(Iterator str, IteratorPointer /*str_end*/)
      {
        static_assert(rsl::is_floating_point_v<T>, "T must be a signed type");

        int32 sign = 1;

        auto c = str;

        if(*c == '-')
        {
          sign = -1;
          ++c;
        }
        else if(*c == '+')
        {
          ++c;
        }

        T before_radix_value          = 0.0f;
        T after_radix_value           = 0.0f;
        card32 num_digits_after_radix = 0;
        bool assigning_before_radix   = true;
        const optional<T> result      = nullopt;

        while(*c != '\0')
        {
          if(!is_digitf(*c))
          {
            break;
          }

          if(*c == '.')
          {
            assigning_before_radix = false;
            ++c;
            continue;
          }

          if(assigning_before_radix)
          {
            before_radix_value = ctoi(*c) + before_radix_value * 10.0f;
          }
          else
          {
            after_radix_value = ctoi(*c) + after_radix_value * 10.0f;
            ++num_digits_after_radix;
          }
          ++c;
        }

        // if (str_end)
        //{
        //   *str_end = const_cast<Iterator>(str);
        // }
        return optional<T>(sign * before_radix_value + (after_radix_value / ((rsl::max)(1.0f, pow(10.0f, num_digits_after_radix)))));
      }

      template <typename T, typename Iterator>
      constexpr optional<T> str_to_floating_point(Iterator str)
      {
        return str_to_floating_point<T>(str, static_cast<Iterator*>(nullptr));
      }

      template <typename Iterator, typename IteratorPointer>
      constexpr optional<bool> str_to_bool(Iterator str, IteratorPointer strEnd)
      {
        constexpr Iterator true_str[]  = "true";  // NOLINT(modernize-avoid-c-arrays)
        constexpr Iterator false_str[] = "false"; // NOLINT(modernize-avoid-c-arrays)

        if(string_equals(str, true_str, size(true_str)))
        {
          // if (str_end)
          //{
          //   *str_end = str + size(true_str);
          // }
          return true;
        }
        else if(string_equals(str, false_str, size(false_str)))
        {
          // if (str_end)
          //{
          //   *str_end = str + size(false_str);
          // }
          return false;
        }

        return str_to_unsigned(str, strEnd, 10);
      }

      template <typename T, typename Iterator>
      constexpr optional<T> str_to_bool(const Iterator str)
      {
        return str_to_bool<T>(str, static_cast<Iterator*>(nullptr));
      }

      // iterator can be of type:
      // - char8*
      // - tchar*
      // - istreambuf_iterator
      // - random_access_iterator
      template <typename Iterator, typename IteratorPointer>
      constexpr optional<void*> str_to_pointer(Iterator str, IteratorPointer strEnd)
      {
        optional<uint64> opt_value(str_to_unsigned(str, strEnd));
        void* val = opt_value.has_value() ? reinterpret_cast<void*>(*opt_value) : nullptr; // NOLINT(performance-no-int-to-ptr, cppcoreguidelines-pro-type-reinterpret-cast)
        return optional<void*>(val);
      }

      template <typename Iterator>
      constexpr optional<void*> str_to_pointer(const Iterator str)
      {
        return str_to_pointer(str, static_cast<Iterator*>(nullptr));
      }

      namespace string_utils
      {
        // compares 2 strings lexicographically
        template <typename Traits, typename Pointer, typename SizeType>
        int32 compare(Pointer lhs, Pointer rhs, SizeType lhsLength, SizeType rhsLength)
        {
          const int32 result = Traits::compare(lhs, rhs, (rsl::min)(lhsLength, rhsLength));

          if(result != 0)
          {
            return result;
          }
          if(lhsLength < rhsLength)
          {
            return -1;
          }
          if(lhsLength > rhsLength)
          {
            return 1;
          }
          return 0;
        }
        // finds the first substring [str, str + toFindLength) within [lhsStr, lhsStr + lhsLength), starting from pos
        template <typename Traits, typename Pointer, typename SizeType>
        SizeType find(Pointer lhsStr, SizeType lhsLength, SizeType pos, Pointer toFindStr, SizeType toFindLength, SizeType defaultValue)
        {
          REX_ASSERT_X(pos < lhsLength, "pos out of bounds");

          // substring must be found between [pos, size() - toFindLength)
          // we subtract the length of the string to find for optimization
          Pointer start         = lhsStr + pos;
          const Pointer lhs_end = lhsStr + lhsLength;

          const Pointer end = lhs_end - toFindLength + 1;
          start             = Traits::find(start, static_cast<count_t>(end - start), *toFindStr);

          if(start != nullptr)
          {
            while(start != end)
            {
              if(Traits::compare(start + 1, toFindStr + 1, toFindLength - 1) == 0)
              {
                return static_cast<SizeType>(start - lhsStr);
              }
              ++start;
              start = Traits::find(start, static_cast<count_t>(lhs_end - start), *toFindStr);
            }
          }
          return defaultValue;
        }
        // finds the last substring [str, str + toFindLength) within [lhsStr, lhsStr + lhsLength), starting from pos
        template <typename Traits, typename Pointer, typename SizeType>
        SizeType rfind(Pointer lhsStr, SizeType lhsLength, SizeType pos, Pointer toFindStr, SizeType toFindLength, SizeType defaultValue)
        {
          pos = (rsl::min)(pos, lhsLength - 1);
          REX_ASSERT_X(pos < lhsLength, "pos out of bounds");

          // the string must be found between [begin, pos]

          // we'll first check where the last char in the substring is found in the string
          Pointer start              = lhsStr + pos;
          const Pointer end          = (lhsStr - 1) + toFindLength - 1;
          const Pointer to_find_last = toFindStr + toFindLength - 1;
          start                      = Traits::rfind(start, static_cast<count_t>(start - end), *to_find_last);

          if(start != nullptr)
          {
            while(start != end)
            {
              const Pointer new_start = start - (toFindLength - 1);
              if(Traits::compare(new_start, toFindStr, toFindLength - 1) == 0)
              {
                return static_cast<SizeType>(new_start - lhsStr);
              }
              --start;
              start = Traits::rfind(start, static_cast<count_t>(start - end), *to_find_last);
            }
          }
          return defaultValue;
        }
        // finds the first occurrence of a char in the substring [str, str + length) within [lhsStr, lhsStr + lhsLength), starting from pos
        template <typename Traits, typename Pointer, typename SizeType>
        SizeType find_first_of(Pointer lhsStr, SizeType lhsLength, SizeType pos, Pointer rhsStr, SizeType rhsLength, SizeType defaultValue)
        {
          const character_lookup<typename Traits::char_type> lookup(rhsStr, rhsLength);

          for(SizeType i = pos; i < lhsLength; ++i)
          {
            auto c = lhsStr[i];
            if(lookup.exists(c))
            {
              return i;
            }
          }

          return defaultValue;
        }
        // finds the last occurrence of a char in the substring [rhsStr, rhsStr + rhsLength) within [lhsStr, lhsStr + lhsLength), starting from pos
        template <typename Traits, typename Pointer, typename SizeType>
        SizeType find_last_of(Pointer lhsStr, SizeType lhsLength, SizeType pos, Pointer rhsStr, SizeType rhsLength, SizeType defaultValue)
        {
          const character_lookup<typename Traits::char_type> lookup(rhsStr, rhsLength);

          for(SizeType i = lhsLength; i >= pos; --i)
          {
            auto c = lhsStr[i];
            if(lookup.exists(c))
            {
              return i;
            }
          }

          return defaultValue;
        }
        // finds the first occurrence of a char not in the substring [str, str + length) within [lhsStr, lhsStr + lhsLength), starting from pos
        template <typename Traits, typename Pointer, typename SizeType>
        SizeType find_first_not_of(Pointer lhsStr, SizeType lhsLength, SizeType pos, Pointer rhsStr, SizeType rhsLength, SizeType defaultValue)
        {
          const character_lookup<typename Traits::char_type> lookup(rhsStr, rhsLength);

          for(SizeType i = pos; i < lhsLength; ++i)
          {
            auto c = lhsStr[i];
            if(!lookup.exists(c))
            {
              return i;
            }
          }

          return defaultValue;
        }
        // finds the last occurrence of a char not in the substring [str, str + length) within [lhsStr, lhsStr + lhsLength), starting from pos
        template <typename Traits, typename Pointer, typename SizeType>
        SizeType find_last_not_of(Pointer lhsStr, SizeType lhsLength, SizeType pos, Pointer rhsStr, SizeType rhsLength, SizeType defaultValue)
        {
          const character_lookup<typename Traits::char_type> lookup(rhsStr, rhsLength);

          for(SizeType i = lhsLength; i >= pos; --i)
          {
            auto c = lhsStr[i];
            if(!lookup.exists(c))
            {
              return i;
            }
          }

          return defaultValue;
        }
      } // namespace string_utils
    }   // namespace internal

    /// RSL Comment: Different from ISO C++ Standard at time of writing (17/Jul/2022)
    // returns an optional instead of a basic type
    // converts a byte string to a floating point value
    REX_NO_DISCARD constexpr optional<float64> atof(const char8* str)
    {
      return internal::str_to_floating_point<float64>(str);
    }

    /// RSL Comment: Different from ISO C++ Standard at time of writing (17/Jul/2022)
    // this returns an optional instead of a basic type
    // converts a byte string to an integer value
    REX_NO_DISCARD constexpr optional<int32> atoi(const char8* str)
    {
      // this is the same function as strtoi, but is less flexible
      return internal::str_to_signed<int32>(str, 10);
    }
    /// RSL Comment: Different from ISO C++ Standard at time of writing (17/Jul/2022)
    // this returns an optional instead of a basic type
    // converts a byte string to an integer value
    REX_NO_DISCARD constexpr optional<long> atol(const char8* str)
    {
      // this is the same function as strtol, but is less flexible
      return internal::str_to_signed<long>(str, 10);
    }
    /// RSL Comment: Different from ISO C++ Standard at time of writing (17/Jul/2022)
    // this returns an optional instead of a basic type
    // converts a byte string to an integer value
    REX_NO_DISCARD constexpr optional<int64> atoll(const char8* str)
    {
      // this is the same function as strtoll, but is less flexible
      return internal::str_to_signed<int64>(str, 10);
    }

    /// RSL Comment: Not in ISO C++ Standard at time of writing (10/Jul/2022)
    // converts a byte string to an integer value
    REX_NO_DISCARD constexpr optional<int32> strtoi(const char8* str, char8** strEnd, int32 base)
    {
      return internal::str_to_signed<int32>(str, strEnd, base);
    }
    /// RSL Comment: Different from ISO C++ Standard at time of writing (17/Jul/2022)
    // this returns an optional instead of a basic type
    // converts a byte string to an integer value
    REX_NO_DISCARD constexpr optional<long> strtol(const char8* str, char8** strEnd, int32 base)
    {
      return internal::str_to_signed<long>(str, strEnd, base);
    }
    // converts a byte string to an integer value
    // this returns an optional instead of a basic type
    REX_NO_DISCARD constexpr optional<int64> strtoll(const char8* str, char8** strEnd, int32 base)
    {
      return internal::str_to_signed<int64>(str, strEnd, base);
    }

    /// RSL Comment: Not in ISO C++ Standard at time of writing (10/Jul/2022)
    // converts a byte string to an unsigned integer value
    REX_NO_DISCARD constexpr optional<uint32> strtoui(const char8* str, char8** strEnd, int32 base)
    {
      return internal::str_to_unsigned<uint32>(str, strEnd, base);
    }
    // converts a byte string to an unsigned integer value
    REX_NO_DISCARD constexpr optional<ulong> strtoul(const char8* str, char8** strEnd, int32 base)
    {
      return internal::str_to_unsigned<ulong>(str, strEnd, base);
    }
    // converts a byte string to an unsigned integer value
    REX_NO_DISCARD constexpr optional<uint64> strtoull(const char8* str, char8** strEnd, int32 base)
    {
      return internal::str_to_unsigned<uint64>(str, strEnd, base);
    }

    /// RSL Comment: Different from ISO C++ Standard at time of writing (27/Aug/2022)
    // this returns an optional instead of a basic type
    // converts a byte string to an floating point value
    REX_NO_DISCARD constexpr optional<float32> strtof(const char8* str, char8** strEnd)
    {
      return internal::str_to_floating_point<float32>(str, strEnd);
    }
    /// RSL Comment: Different from ISO C++ Standard at time of writing (27/Aug/2022)
    // this returns an optional instead of a basic type
    // converts a byte string to an floating point value
    REX_NO_DISCARD constexpr optional<float64> strtod(const char8* str, char8** strEnd)
    {
      return internal::str_to_floating_point<float64>(str, strEnd);
    }
    /// RSL Comment: Different from ISO C++ Standard at time of writing (27/Aug/2022)
    // this returns an optional instead of a basic type
    // converts a byte string to an floating point value
    REX_NO_DISCARD constexpr optional<lfloat64> strtold(const char8* str, char8** strEnd)
    {
      return internal::str_to_floating_point<lfloat64>(str, strEnd);
    }

    //
    // string Manipulation (cstring.h)
    //

    //
    // string Examination (cstring.h)
    //

    //
    // Miscellaneous (cstring.h)
    //

    // returns a text version of a given error code
    // REX_NO_DISCARD const char8* strerror(int32 errnum);

  } // namespace v1
} // namespace rsl

#include "rex_std/bonus/string/string_utils_impl.h"