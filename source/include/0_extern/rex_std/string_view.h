// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: string_view.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/compiler.h"
#include "rex_std/bonus/defines.h"
#include "rex_std/bonus/string/string_utils.h"
#include "rex_std/bonus/types.h"
#include "rex_std/bonus/utility/element_literal.h"
#include "rex_std/internal/algorithm/min.h"
#include "rex_std/internal/assert/assert_fwd.h"
#include "rex_std/internal/functional/hash.h"
#include "rex_std/internal/iterator/begin.h"
#include "rex_std/internal/iterator/end.h"
#include "rex_std/internal/iterator/iterator_tags.h"
#include "rex_std/internal/iterator/iterator_traits.h"
#include "rex_std/internal/iterator/reverse_iterator.h"
#include "rex_std/internal/memory/addressof.h"
#include "rex_std/internal/memory/memcpy.h"
#include "rex_std/internal/memory/nullptr.h"
#include "rex_std/internal/type_traits/is_base_of.h"
#include "rex_std/internal/utility/swap.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename CharType, typename Traits = char_traits<CharType>>
    class basic_string_view
    {
    public:
      using traits_type            = Traits;
      using value_type             = CharType;
      using pointer                = value_type*;
      using const_pointer          = const value_type*;
      using reference              = value_type&;
      using const_reference        = const value_type&;
      using const_iterator         = const_random_access_iterator<value_type>;
      using iterator               = const_iterator;
      using const_reverse_iterator = reverse_iterator<const_iterator>;
      using reverse_iterator       = const_reverse_iterator;
      using size_type              = count_t;
      using difference_type        = int32;

      // Default constructor. Constructs an empty view.
      // after construction, data() is equal to nullptr, size() equals 0
      constexpr basic_string_view()
          : m_data()
          , m_length(0)
      {
      }
      // Copy constructor. defaulted, shallow copies members over from other
      constexpr basic_string_view(const basic_string_view&) = default;
      // Move constructor. defaulted, shallow copies members over from other
      constexpr basic_string_view(basic_string_view&&) = default;
      // Constructs a view of the first count characters starting at s
      constexpr basic_string_view(const_pointer s, size_type count)
          : m_data(s)
          , m_length(count)
      {
      }
      // Constructs a view starting at s, reaching to the first null termination char.
      constexpr explicit basic_string_view(const const_pointer& s)
          : m_data(s)
          , m_length(traits_type::length(s))
      {
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (07/Jul/2022)
      // In the Standard, last is a different template type than first.
      // Constructs a view using a continuous iterator
      template <typename It1, typename It2, enable_if_t<is_base_of_v<input_iterator_tag, typename It2::iterator_category>, bool> = true>
      constexpr basic_string_view(It1 first, It2 last)
          : m_data(rsl::iterator_to_pointer(first))
          , m_length(last - first)
      {
      }
      /// RSL Comment: Not in ISO C++ Standard at time of writing (07/Jul/2022)
      // The standard doesn't provide an overload for a string literal
      // Constructs a view using a string literal
      template <size_t Size>
      constexpr basic_string_view(const value_type (&s)[Size]) // NOLINT(google-explicit-constructor, modernize-avoid-c-arrays)
          : m_data(s)
          , m_length(Size - 1)
      {
      }
      // can't construct a string view from a nullptr
      constexpr basic_string_view(rsl::nullptr_t) = delete;

      // default destructor
      ~basic_string_view() = default;

      // assigns a new view
      constexpr basic_string_view& operator=(const basic_string_view&) = default;
      // move a view, this is just a copy
      constexpr basic_string_view& operator=(basic_string_view&&) = default;

      // returns an iterator to the beginning of the view
      constexpr const_iterator begin() const
      {
        return const_iterator(m_data);
      }
      // returns an iterator to the beginning of the view
      constexpr const_iterator cbegin() const
      {
        return begin();
      }
      // returns an iterator to the character following the last character
      constexpr const_iterator end() const
      {
        return const_iterator(m_data + m_length);
      }
      // returns an iterator to the character following the last character
      constexpr const_iterator cend() const
      {
        return end();
      }
      // returns a reverse iterator to the first character in the reversed view
      constexpr const_reverse_iterator rbegin() const
      {
        return const_reverse_iterator(end());
      }
      // returns a reverse iterator to the first character in the reversed view
      constexpr const_reverse_iterator crbegin() const
      {
        return rbegin();
      }
      // returns a reverse iterator to the character following the last in the reversed view
      constexpr const_reverse_iterator rend() const
      {
        return const_reverse_iterator(begin());
      }
      // returns a reverse iterator to the character following the last in the reversed view
      constexpr const_reverse_iterator crend() const
      {
        return rend();
      }

      //
      // element access
      //

      // returns a const reference to the character at pos
      // bounds checking is not performed
      constexpr const_reference operator[](const size_type pos) const
      {
        return m_data[pos];
      }
      // returns a const reference to the character at pos
      // bounds checking is performed
      constexpr const_reference at(const size_type pos) const
      {
        REX_ASSERT_X(pos < m_length, "string view index out of bounds");
        return m_data[pos];
      }
      // returns a const reference to the first character in the view
      constexpr const_reference front() const
      {
        return m_data[0];
      }
      // returns a const reference to the last character in the view
      constexpr const_reference back() const
      {
        return m_data[size() - 1];
      }
      // returns a pointer to the first character of the view
      constexpr const_pointer data() const
      {
        return m_data;
      }

      // returns the number of elements in the view
      constexpr size_type size() const
      {
        return m_length;
      }
      // returns the number of elements in the view
      constexpr size_type length() const
      {
        return m_length;
      }
      // returns the max possible number of elements the view can hold
      constexpr size_type max_size() const
      {
        return (rsl::numeric_limits<difference_type>::max)();
      }
      // checks if the view is empty
      constexpr bool empty() const
      {
        return size() == 0;
      }

      // moves the start of the view forward by n characters
      constexpr void remove_prefix(const size_type n)
      {
        m_data += n;
        m_length -= n;
      }
      // move the end of the view backward by n characters
      constexpr void remove_suffix(const size_type n)
      {
        m_length -= n;
      }
      // exchanges the view with that of v
      constexpr void swap(basic_string_view& other)
      {
        rsl::swap(m_data, other.m_data);
        rsl::swap(m_length, other.m_length);
      }
      // copies the substring [pos, pos + count) to the character array pointed to by dest
      constexpr size_type copy(pointer dst, size_type count, size_type pos = 0) const
      {
        count = count == s_npos ? m_length - pos : count;
        rsl::memcpy(dst, m_data + pos, sizeof(CharType) * count);
        return count;
      }
      // returns a view of the substring [pos, pos + count)
      REX_NO_DISCARD constexpr basic_string_view substr(size_type pos = 0, size_type count = s_npos) const
      {
        count = count == s_npos ? m_length - pos : count;
        return basic_string_view(m_data + pos, count);
      }
      // compares 2 character sequences
      REX_NO_DISCARD constexpr int32 compare(basic_string_view sv) const
      {
        return internal::string_utils::compare<traits_type>(data(), sv.data(), size(), sv.size());
      }
      // compares 2 characters sequences
      REX_NO_DISCARD constexpr int32 compare(size_type pos1, size_type count1, basic_string_view v) const
      {
        return substr(pos1, count1).compare(v);
      }
      // compares 2 character sequences
      REX_NO_DISCARD constexpr int32 compare(size_type pos1, size_type count1, basic_string_view v, size_type pos2, size_type count2) const
      {
        return substr(pos1, count1).compare(v.substr(pos2, count2));
      }
      /// RSL Comment: Not in ISO C++ Standard at time of writing (08/Jul/2022)
      // The standard doesn't provide an overload for a string literal
      // compares 2 character sequences
      template <count_t Size>
      REX_NO_DISCARD constexpr int32 compare(const value_type (&s)[Size]) const // NOLINT(modernize-avoid-c-arrays)
      {
        return compare(basic_string_view(s, Size - 1));
      }
      /// RSL Comment: Not in ISO C++ Standard at time of writing (08/Jul/2022)
      // The standard doesn't provide an overload for a string literal
      // compares 2 character sequences
      template <count_t Size>
      REX_NO_DISCARD constexpr int32 compare(size_type pos1, size_type count1, const value_type (&s)[Size]) const // NOLINT(modernize-avoid-c-arrays)
      {
        return substr(pos1, count1).compare(basic_string_view(s, Size - 1));
      }
      // compares 2 character sequences
      REX_NO_DISCARD constexpr int32 compare(size_type pos1, size_type count1, const_pointer s, size_type count2) const
      {
        return substr(pos1, count1).compare(basic_string_view(s, count2));
      }

      // checks if the string view starts with the given prefix
      REX_NO_DISCARD constexpr bool starts_with(const basic_string_view sv) const
      {
        return traits_type::compare(data(), sv.data(), sv.length()) == 0;
      }
      // checks if the string view starts with the given prefix
      REX_NO_DISCARD constexpr bool starts_with(const value_type c) const
      {
        return traits_type::eq(front(), c);
      }
      // checks if the string view starts with the given prefix
      REX_NO_DISCARD constexpr bool starts_with(const_pointer& s) const
      {
        return starts_with(basic_string_view(s));
      }
      // checks if the string view ends with the given suffix
      REX_NO_DISCARD constexpr bool ends_with(const basic_string_view rhs) const
      {
        return internal::string_utils::compare<traits_type>(data() + (length() - rhs.size()), rhs.data(), rhs.length(), rhs.length()) == 0;
      }
      // checks if the string view ends with the given suffix
      REX_NO_DISCARD constexpr bool ends_with(const_pointer& s) const
      {
        return ends_with(basic_string_view(s));
      }
      // checks if the string view ends with the given suffix
      REX_NO_DISCARD constexpr bool ends_with(const CharType c) const
      {
        return !empty() && traits_type::eq(back(), c);
      }

      // checks if the string view contains the given substring
      REX_NO_DISCARD constexpr bool contains(basic_string_view sv) const
      {
        return find(sv) != s_npos;
      }
      // checks if the string view contains the given substring
      REX_NO_DISCARD constexpr bool contains(value_type c) const
      {
        return find(c) != s_npos;
      }
      // checks if the string view contains the given substring
      REX_NO_DISCARD constexpr bool contains(const_pointer& s) const
      {
        return contains(basic_string_view(s));
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // because we have the above function, we don't define the following
      // bool contains(const_pointer Size) const;

      //
      // search
      //

      // finds the first substring equal to the given character sequence in this view
      constexpr size_type find(basic_string_view view, card32 pos = 0) const
      {
        return rsl::internal::string_utils::find<traits_type, const_pointer>(m_data, length(), pos, view.data(), view.length(), s_npos);
      }
      // finds the first substring equal to the given character sequence in this view
      constexpr size_type find(value_type c, card32 pos = 0) const
      {
        return internal::string_utils::find<traits_type, const_pointer>(m_data, length(), pos, rsl::addressof(c), 1_elem, s_npos);
      }
      // finds the first substring equal to the given character sequence in this view
      constexpr size_type find(const_pointer str, card32 pos, card32 count) const
      {
        return internal::string_utils::find<traits_type, const_pointer>(m_data, length(), pos, str, count, s_npos);
      }
      // finds the first substring equal to the given character sequence in this view
      constexpr size_type find(const_pointer str, card32 pos = 0) const
      {
        return internal::string_utils::find<traits_type, const_pointer>(m_data, length(), pos, str, traits_type::length(str), s_npos);
      }

      // finds the last substring equal to the given character sequence in this view
      constexpr size_type rfind(basic_string_view view, card32 pos = s_npos) const
      {
        if(pos == s_npos)
        {
          pos = length();
        }
        return internal::string_utils::rfind<traits_type, const_pointer>(m_data, length(), pos, view.data(), view.length(), s_npos);
      }
      // finds the last substring equal to the given character sequence in this view
      constexpr size_type rfind(value_type c, card32 pos = s_npos) const
      {
        if(pos == s_npos)
        {
          pos = length();
        }
        return internal::string_utils::rfind<traits_type, const_pointer>(m_data, length(), pos, rsl::addressof(c), 1_elem, s_npos);
      }
      // finds the last substring equal to the given character sequence in this view
      constexpr size_type rfind(const_pointer str, card32 pos, card32 count) const
      {
        return internal::string_utils::rfind<traits_type, const_pointer>(m_data, length(), pos, str, count, s_npos);
      }
      // finds the last substring equal to the given character sequence in this view
      constexpr size_type rfind(const_pointer str, card32 pos = s_npos) const
      {
        if(pos == s_npos)
        {
          pos = length();
        }
        return internal::string_utils::rfind<traits_type, const_pointer>(m_data, length(), pos, str, traits_type::length(str), s_npos);
      }

      // finds the first character equal to any of the characters in the given character sequence in this view.
      constexpr size_type find_first_of(basic_string_view view, card32 pos = 0) const
      {
        return internal::string_utils::find_first_of<traits_type, const_pointer>(m_data, length(), pos, view.data(), view.length(), s_npos);
      }
      // finds the first character equal to any of the characters in the given character sequence in this view.
      constexpr size_type find_first_of(value_type c, card32 pos = 0) const
      {
        return internal::string_utils::find_first_of<traits_type, const_pointer>(m_data, length(), pos, rsl::addressof(c), 1_elem, s_npos);
      }
      // finds the first character equal to any of the characters in the given character sequence in this view.
      constexpr size_type find_first_of(const_pointer str, card32 pos, card32 count) const
      {
        return internal::string_utils::find_first_of<traits_type, const_pointer>(m_data, length(), pos, str, count, s_npos);
      }
      // finds the first character equal to any of the characters in the given character sequence in this view.
      constexpr size_type find_first_of(const_pointer str, card32 pos = 0) const
      {
        return internal::string_utils::find_first_of<traits_type, const_pointer>(m_data, length(), pos, str, traits_type::length(str), s_npos);
      }

      // finds the last character equal to any of the characters in the given character sequence in this view.
      constexpr size_type find_last_of(basic_string_view view, card32 pos = 0) const
      {
        return internal::string_utils::find_last_of<traits_type, const_pointer>(m_data, length(), pos, view.data(), view.length(), s_npos);
      }
      // finds the last character equal to any of the characters in the given character sequence in this view.
      constexpr size_type find_last_of(value_type c, card32 pos = 0) const
      {
        return internal::string_utils::find_last_of<traits_type, const_pointer>(m_data, length(), pos, rsl::addressof(c), 1_elem, s_npos);
      }
      // finds the last character equal to any of the characters in the given character sequence in this view.
      constexpr size_type find_last_of(const_pointer str, card32 pos, card32 count) const
      {
        return internal::string_utils::find_last_of<traits_type, const_pointer>(m_data, length(), pos, str, count, s_npos);
      }
      // finds the last character equal to any of the characters in the given character sequence in this view.
      constexpr size_type find_last_of(const_pointer str, card32 pos = 0) const
      {
        return internal::string_utils::find_last_of<traits_type, const_pointer>(m_data, length(), pos, str, traits_type::length(str), s_npos);
      }

      // finds the first character not equal to any of the characters in the given character sequence in this view.
      constexpr size_type find_first_not_of(basic_string_view view, card32 pos = 0) const
      {
        return internal::string_utils::find_first_not_of<traits_type, const_pointer>(m_data, length(), pos, view.data(), view.length(), s_npos);
      }
      // finds the first character not equal to any of the characters in the given character sequence in this view.
      constexpr size_type find_first_not_of(value_type c, card32 pos = 0) const
      {
        return internal::string_utils::find_first_not_of<traits_type, const_pointer>(m_data, length(), pos, rsl::addressof(c), 1_elem, s_npos);
      }
      // finds the first character not equal to any of the characters in the given character sequence in this view.
      constexpr size_type find_first_not_of(const_pointer str, card32 pos, card32 count) const
      {
        return internal::string_utils::find_first_not_of<traits_type, const_pointer>(m_data, length(), pos, str, count, s_npos);
      }
      // finds the first character not equal to any of the characters in the given character sequence in this view.
      constexpr size_type find_first_not_of(const_pointer str, card32 pos = 0) const
      {
        return internal::string_utils::find_first_not_of<traits_type, const_pointer>(m_data, length(), pos, str, traits_type::length(str), s_npos);
      }

      // finds the last character not equal to any of the characters in the given character sequence in this view.
      constexpr size_type find_last_not_of(basic_string_view view, card32 pos = 0) const
      {
        return internal::string_utils::find_last_not_of<traits_type, const_pointer>(m_data, length(), pos, view.data(), view.length(), s_npos);
      }
      // finds the last character not equal to any of the characters in the given character sequence in this view.
      constexpr size_type find_last_not_of(value_type c, card32 pos = 0) const
      {
        return internal::string_utils::find_last_not_of<traits_type, const_pointer>(m_data, length(), pos, rsl::addressof(c), 1_elem, s_npos);
      }
      // finds the last character not equal to any of the characters in the given character sequence in this view.
      constexpr size_type find_last_not_of(const_pointer str, card32 pos, card32 count) const
      {
        return internal::string_utils::find_last_not_of<traits_type, const_pointer>(m_data, length(), pos, str, count, s_npos);
      }
      // finds the last character not equal to any of the characters in the given character sequence in this view.
      constexpr size_type find_last_not_of(const_pointer str, card32 pos = 0) const
      {
        return internal::string_utils::find_last_not_of<traits_type, const_pointer>(m_data, length(), pos, str, traits_type::length(str), s_npos);
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (11/Jul/2022)
      // npos is a function in RSL, not access to a public static variable
      // special value, the meaning is different depending on the context.
      // usually it means to indicate that something is not found.
      constexpr static card32 npos()
      {
        return s_npos;
      }

    private:
      // compares 2 strings lexicographically
      int32 compare(pointer lhs, pointer rhs, size_type lhsLength, size_type rhsLength) const
      {
        int32 res = traits_type::compare(lhs, rhs, (rsl::min)(lhsLength, rhsLength));

        if(res != 0)
        {
          return res;
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

    private:
      const_pointer m_data;
      size_type m_length;

      constexpr static card32 s_npos = -1;
    };

    // compares if 2 string views are equal
    template <typename CharType, typename Traits>
    constexpr bool operator==(basic_string_view<CharType, Traits> lhs, basic_string_view<CharType, Traits> rhs)
    {
      if(lhs.length() != rhs.length())
      {
        return false;
      }

      return Traits::compare(lhs.data(), rhs.data(), lhs.length()) == 0;
    }

    template <typename CharType, typename Traits, count_t Size>
    constexpr bool operator==(basic_string_view<CharType, Traits> lhs, const CharType (&rhs)[Size]) // NOLINT(modernize-avoid-c-arrays)
    {
      return lhs == rsl::basic_string_view<CharType, Traits>(rhs);
    }

    // compares if 2 string views are not equal
    template <typename CharType, typename Traits>
    constexpr bool operator!=(basic_string_view<CharType, Traits> lhs, basic_string_view<CharType, Traits> rhs)
    {
      return !(lhs == rhs);
    }

    template <typename CharType, typename Traits, count_t Size>
    constexpr bool operator!=(basic_string_view<CharType, Traits> lhs, const CharType (&rhs)[Size]) // NOLINT(modernize-avoid-c-arrays)
    {
      return lhs != rsl::basic_string_view<CharType, Traits>(rhs);
    }

    // lexicographically compares 2 string views
    template <typename CharType, typename Traits>
    constexpr bool operator<(basic_string_view<CharType, Traits> lhs, basic_string_view<CharType, Traits> rhs)
    {
      return lhs.compare(rhs) < 0;
    }
    template <typename CharType, typename Traits, count_t Size>
    constexpr bool operator<(basic_string_view<CharType, Traits> lhs, const CharType (&rhs)[Size]) // NOLINT(modernize-avoid-c-arrays)
    {
      return lhs.compare(rhs) < 0;
    }

    // lexicographically compares 2 string views
    template <typename CharType, typename Traits>
    constexpr bool operator<=(basic_string_view<CharType, Traits> lhs, basic_string_view<CharType, Traits> rhs)
    {
      return lhs.compare(rhs) <= 0;
    }
    template <typename CharType, typename Traits, count_t Size>
    constexpr bool operator<=(basic_string_view<CharType, Traits> lhs, const CharType (&rhs)[Size]) // NOLINT(modernize-avoid-c-arrays)
    {
      return lhs.compare(rhs) <= 0;
    }
    // lexicographically compares 2 string views
    template <typename CharType, typename Traits>
    constexpr bool operator>(basic_string_view<CharType, Traits> lhs, basic_string_view<CharType, Traits> rhs)
    {
      return lhs.compare(rhs) > 0;
    }
    template <typename CharType, typename Traits, count_t Size>
    constexpr bool operator>(basic_string_view<CharType, Traits> lhs, const CharType (&rhs)[Size]) // NOLINT(modernize-avoid-c-arrays)
    {
      return lhs.compare(rhs) > 0;
    }
    // lexicographically compares 2 string views
    template <typename CharType, typename Traits>
    constexpr bool operator>=(basic_string_view<CharType, Traits> lhs, basic_string_view<CharType, Traits> rhs)
    {
      return lhs.compare(rhs) >= 0;
    }
    template <typename CharType, typename Traits, count_t Size>
    constexpr bool operator>=(basic_string_view<CharType, Traits> lhs, const CharType (&rhs)[Size]) // NOLINT(modernize-avoid-c-arrays)
    {
      return lhs.compare(rhs) >= 0;
    }
    // stores each characters from the resulting sequence to the output stream
     template <typename CharType, typename Traits>
     class basic_ostream;

     template <typename CharT, typename Traits>
     class basic_streambuf;

     template <typename CharType, typename Traits>
     basic_ostream<CharType, Traits>& operator<<(basic_ostream<CharType, Traits>& os, const basic_string_view<CharType, Traits>& str)
     {
        os.rdbuf()->sputn(str.data(), str.length());
        return os;
     }

    using string_view   = basic_string_view<char8>;
    using WStringView   = basic_string_view<tchar>;
    using u16StringView = basic_string_view<char16_t>;
    using u32StringView = basic_string_view<char32_t>;

    namespace string_view_literals
    {
#if defined(REX_MSVC_COMPILER)
  #pragma warning(push)
  #pragma warning(disable : 4455) // literal suffix identifiers that do not start with an underscore are reserved
#elif defined(REX_CLANG_COMPILER)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wuser-defined-literals" // literal suffix identifiers that do not start with an underscore are reserved
#endif
      // returns a string view of the desired type
      constexpr string_view operator""s(const char8* s, size_t len) // NOLINT(clang-diagnostic-user-defined-literals)
      {
        const count_t len_as_count = static_cast<count_t>(len);
        return string_view(s, len_as_count);
      }
      // returns a string view of the desired type
      constexpr u16StringView operator""s(const char16_t* s, size_t len) // NOLINT(clang-diagnostic-user-defined-literals)
      {
        const count_t len_as_count = static_cast<count_t>(len);
        return u16StringView(s, len_as_count);
      }
      // returns a string view of the desired type
      constexpr u32StringView operator""s(const char32_t* s, size_t len) // NOLINT(clang-diagnostic-user-defined-literals)
      {
        const count_t len_as_count = static_cast<count_t>(len);
        return u32StringView(s, len_as_count);
      }
      // returns a string view of the desired type
      constexpr WStringView operator""s(const tchar* s, size_t len) // NOLINT(clang-diagnostic-user-defined-literals)
      {
        const count_t len_as_count = static_cast<count_t>(len);
        return WStringView(s, len_as_count);
      }
#if defined(REX_MSVC_COMPILER)
  #pragma warning(pop)
#elif defined(REX_CLANG_COMPILER)
  #pragma clang diagnostic pop
#endif
    } // namespace string_view_literals

    template <typename CharType, typename Traits>
    struct hash<basic_string_view<CharType, Traits>>
    {
      hash_result operator()(const basic_string_view<CharType, Traits>& str) const
      {
        return rsl::internal::hash(str.data(), str.length());
      }
    };

    template <typename It>
    basic_string_view(It, It) -> basic_string_view<typename rsl::iterator_traits<It>::value_type, rsl::char_traits<typename rsl::iterator_traits<It>::value_type>>;

  } // namespace v1
} // namespace rsl
