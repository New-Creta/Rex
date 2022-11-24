// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: char_traits.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/string/string_utils.h"
#include "rex_std/bonus/types.h"
#include "rex_std/cstring.h"
#include "rex_std/internal/ios/fpos.h"
#include "rex_std/internal/ios/io_types.h"
#include "rex_std/internal/wchar/mbstate.h"

namespace rsl
{
  inline namespace v1
  {

    // char_traits are implement and used by the basic_string class in Rex STL.
    // Unlike EASTL, we do use char_traits, this is so a basic_string can be created
    // using a case-insensitive char_traits to make case-insensitive usage easier.
    template <typename CharT>
    class char_traits
    {
    };

    namespace internal
    {
      template <typename CharT, typename IntType>
      class char_traits_base
      {
      public:
        using char_type = CharT;
        using int_type  = IntType;

        // Assigns character a to character r.
        constexpr static void assign(char_type& d, const char_type& s)
        {
          d = s;
        }
        // Assigns character a to each character in count characters in the character sequence pointed to by p.
        constexpr static char_type* assign(char_type* p, card32 count, char_type a)
        {
          while(count > 0)
          {
            *p++ = a;
            --count;
          }
          return p;
        }

        // Compares a and b for equality.
        constexpr static bool eq(char_type a, char_type b)
        {
          return a == b;
        }
        // Compares a and b in such a way that they are totally ordered.
        constexpr static bool lt(char_type a, char_type b)
        {
          return a < b;
        }

        // Copies count characters from the character string pointed to by src to the character string pointed to by dest
        constexpr static char_type* move(char_type* dest, const char_type* src, count_t count)
        {
          if(src < dest)
          {
            return copy_backward(dest, src, count);
          }
          else
          {
            return copy(dest, src, count);
          }
        }

        // Copies count characters from the character string pointed to by src to the character string pointed to by dest.
        constexpr static char_type* copy(char_type* dest, const char_type* src, count_t count)
        {
          char_type* dest_copy = dest;

          while(count > 0)
          {
            assign(*dest, *src);
            ++dest;
            ++src;
            --count;
          }

          return dest_copy;
        }

        /// RSL Comment: Not in ISO C++ Standard at time of writing (10/Jul/2022)
        // Copies count character backwards from the character string pointed to by src, to the character string pointed to by dest
        constexpr static char_type* copy_backward(char_type* dest, const char_type* src, count_t count)
        {
          char_type* dest_str      = dest + count;
          const char_type* src_str = src + count;

          count_t actual_amount_to_copy = count + 1; // +1 to also copy the null termination char, if it's there
          while(actual_amount_to_copy > 0)
          {
            assign(*dest_str, *src_str);
            --dest_str;
            --src_str;
            --actual_amount_to_copy;
          }

          return dest;
        }

        /// RSL Comment: Not in ISO C++ Standard at time of writing (09/Aug/2022)
        // the following function is defined as it could give 10% to 50% performance benefits
        // over the comparison where a count is provided.
        // depending on the compiler you use and the length of the string as well how many characters are equal
        // it is possible this one results 10 to 50% slower code than the one where count is provided
        // but these cases were more rare.
        // it's generally recommended that if count is >= the shortest string length
        // of either of the 2 strings, you should use this function instead
        // Compares 2 strings lexicographically
        constexpr static int32 compare(const char_type* s1, const char_type* s2)
        {
          return rsl::strcmp(s1, s2);
        }
        // Compares the first count characters of the character strings s1 and s2. The comparison is done lexicographically.
        constexpr static int32 compare(const char_type* s1, const char_type* s2, count_t count)
        {
          return rsl::strncmp(s1, s2, count);
        }

        // Returns the length of the character sequence pointed to by s, that is, the position of the terminating null character (CharT()).
        constexpr static count_t length(const char_type* s)
        {
          return rsl::strlen(s);
        }

        // Searches for character ch within the first count characters of the sequence pointed to by p.
        static constexpr const char_type* find(const char_type* p, count_t count, const char_type& ch)
        {
          while(count > 0)
          {
            if(eq(*p, ch))
            {
              return p;
            }
            --count;
            ++p;
          }

          return nullptr;
        }

        /// RSL Comment: Not in ISO C++ Standard at time of writing (07/Jul/2022)
        // searches for the last occurrence of ch within the first count characters of the sequence pointed to by p
        static constexpr const char_type* rfind(const char_type* p, count_t count, const char_type& ch)
        {
          while(count > 0)
          {
            if(eq(*p, ch))
            {
              return p;
            }
            --count;
            --p;
          }

          return nullptr;
        }

        // Converts a value of int_type to char_type.
        constexpr static char_type to_char_type(int_type c)
        {
          return static_cast<char_type>(c);
        }

        // Converts a value of char_type to int_type
        constexpr static int_type to_int_type(char_type c)
        {
          return static_cast<int_type>(c);
        }

        // Checks whether two values of type int_type are equal.
        constexpr static bool eq_int_type(int_type c1, int_type c2)
        {
          return c1 == c2;
        }

        // Returns a value not equivalent to any valid value of type char_type.
        constexpr static int_type eof()
        {
          return ~0;
        }

        // Given e, produce a suitable value that is not equivalent to eof.
        constexpr static int_type not_eof(int_type e)
        {
          return eq_int_type(e, eof()) ? to_char_type(!eof()) : e;
        }
      };
    } // namespace internal

    // For some reason, the int_type of char_traits<char> is signed while all the others
    // are not, according to the C++ standard anyway.
    // not sure why this is, possibly because they want to support unicode and they require unsigned
    // types, but when dealing with unicode, it's better to use special class type to represent
    // the values instead of C++ basic types.

    template <>
    class char_traits<char8> : public internal::char_traits_base<char8, int32>
    {
    public:
      using char_type  = char8;
      using int_type   = int32;
      using pos_type   = rsl::streampos;
      using off_type   = rsl::streamoff;
      using state_type = rsl::mbstate_t;

      // Compares a and b for equality.
      constexpr static bool eq(char_type a, char_type b)
      {
        return static_cast<unsigned char>(a) == static_cast<unsigned char>(b);
      }
      // Compares a and b in such a way that they are totally ordered.
      constexpr static bool lt(char_type a, char_type b)
      {
        return static_cast<unsigned char>(a) < static_cast<unsigned char>(b);
      }
    };

    template <>
    class char_traits<tchar> : public internal::char_traits_base<tchar, rsl::wint_t>
    {
    public:
      using char_type  = tchar;
      using int_type   = rsl::wint_t;
      using pos_type   = rsl::wstreampos;
      using off_type   = rsl::streamoff;
      using state_type = rsl::mbstate_t;
    };

    template <>
    class char_traits<char16_t> : public internal::char_traits_base<char16_t, uint16>
    {
    public:
      using char_type  = char16_t;
      using int_type   = uint16;
      using pos_type   = rsl::u16streampos;
      using off_type   = rsl::streamoff;
      using state_type = rsl::mbstate_t;
    };

    template <>
    class char_traits<char32_t> : public internal::char_traits_base<char32_t, uint32>
    {
    public:
      using char_type  = char32_t;
      using int_type   = uint32;
      using pos_type   = rsl::u32streampos;
      using off_type   = rsl::streamoff;
      using state_type = rsl::mbstate_t;
    };

  } // namespace v1
} // namespace rsl
