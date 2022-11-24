// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: stack_string.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/array.h"
#include "rex_std/bonus/algorithm/clamp_max.h"
#include "rex_std/bonus/types.h"
#include "rex_std/initializer_list.h"
#include "rex_std/internal/algorithm/clamp.h"
#include "rex_std/internal/algorithm/copy_n.h"
#include "rex_std/internal/memory/memcpy.h"
#include "rex_std/string_view.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename CharType, card32 StrMaxSize>
    class stack_string
    {
    public:
      static_assert(StrMaxSize > 0, "Can't have a stack string with a size of 0");
      static_assert(rsl::is_integral_v<CharType>, "CharType must be of integral type!");

      using value_type     = CharType;
      using iterator       = typename rsl::array<CharType, StrMaxSize>::iterator;
      using const_iterator = typename rsl::array<CharType, StrMaxSize>::const_iterator;

      stack_string()
          : m_data()
          , m_null_terminator_offset(0)
      {
      }

      template <card32 N>
      explicit stack_string(const value_type str[N]) // NOLINT
          : stack_string()
      {
        static_assert(N < StrMaxSize, "Trying to copy more into a string than is allowed");
        m_null_terminator_offset = rsl::memcpy(m_data.data(), str, sizeof(CharType) * N);
      }

      stack_string(rsl::initializer_list<value_type> str) // NOLINT
          : stack_string()
      {
        const card32 copy_size = rsl::clamp_max(static_cast<card32>(str.size()), StrMaxSize);
        rsl::memcpy(m_data.data(), str.begin(), copy_size);
        m_null_terminator_offset = copy_size;
      }

      explicit stack_string(const value_type* str)
          : stack_string()
      {
        const card32 string_length = rsl::string_length(str);
        const card32 copy_size     = rsl::clamp_max(string_length, StrMaxSize);
        rsl::memcpy(m_data.data(), str, copy_size);
        m_null_terminator_offset = copy_size;
      }

      stack_string(const value_type* str, card32 size)
          : stack_string()
      {
        const card32 copy_size = rsl::clamp_max(size, StrMaxSize);
        rsl::memcpy(m_data.data(), str, copy_size);
        m_null_terminator_offset = copy_size;
      }

      stack_string(basic_string_view<CharType> view) // NOLINT(google-explicit-constructor)
          : stack_string()
      {
        const card32 copy_size = rsl::clamp_max(view.length(), StrMaxSize);
        rsl::memcpy(m_data.data(), view.data(), copy_size);
        m_null_terminator_offset = copy_size;
      }

      template <card32 N>
      stack_string(const stack_string<CharType, N>& str) // NOLINT(google-explicit-constructor)
          : m_data()
          , m_null_terminator_offset(str.length())
      {
        const card32 copy_size = rsl::clamp_max(rsl::string_length(str), StrMaxSize);
        rsl::copy_n(str.data(), copy_size, m_data.data());
      }

      stack_string(rsl::nullptr_t) = delete;

      stack_string& operator=(basic_string_view<CharType> view)
      {
        const card32 new_length = rsl::clamp_max(view.length(), StrMaxSize);
        rsl::memcpy(m_data.data(), view.data(), new_length);
        m_null_terminator_offset = new_length;
        return *this;
      }
      stack_string& operator=(const value_type* str)
      {
        const card32 new_length = rsl::clamp_max(rsl::string_length(str), StrMaxSize);
        rsl::memcpy(m_data.data(), str, new_length);
        m_null_terminator_offset = new_length;
        return *this;
      }
      stack_string& operator=(rsl::nullptr_t) = delete;

      void assign(const value_type* first, const value_type* last)
      {
        const card32 length    = static_cast<card32>(last - first);
        const card32 copy_size = clamp_max(length, StrMaxSize);
        rsl::memcpy(data(), first, copy_size);
        m_null_terminator_offset = copy_size;
      }
      void assign(const value_type* newData, card32 length)
      {
        const card32 copy_size = clamp_max(length, StrMaxSize);
        rsl::memcpy(data(), newData, copy_size);
        m_null_terminator_offset = copy_size;
      }

      card32 length() const
      {
        return m_null_terminator_offset;
      }
      card32 size() const
      {
        return m_null_terminator_offset;
      }
      static constexpr card32 max_size()
      {
        return StrMaxSize;
      }

      value_type* data()
      {
        return m_data.data();
      }
      const value_type* data() const
      {
        return m_data.data();
      }

      const value_type* c_str() const
      {
        return m_data.data();
      }

      auto begin()
      {
        return m_data.begin();
      }
      auto end()
      {
        return begin() + m_null_terminator_offset;
      }
      auto cbegin() const
      {
        return m_data.cbegin();
      }
      auto cend() const
      {
        return m_data.cend();
      }
      auto rbegin()
      {
        return m_data.rbegin() + (max_size() - length());
      }
      auto rend()
      {
        return m_data.rend();
      }
      auto crbegin() const
      {
        return m_data.crbegin() + (max_size() - length());
      }
      auto crend() const
      {
        return m_data.crend();
      }

      auto& operator[](card32 idx)
      {
        return m_data[idx];
      }
      const auto& operator[](card32 idx) const
      {
        return m_data[idx];
      }

      auto at(card32 idx)
      {
        REX_ASSERT_X(idx < m_null_terminator_offset, "Index out of range");
        return m_data.at(idx);
      }
      const auto& at(card32 idx) const
      {
        REX_ASSERT_X(idx < m_null_terminator_offset, "Index out of range");
        return m_data.at(idx);
      }

      auto& front()
      {
        return m_data.front();
      }
      const auto& front() const
      {
        return m_data.front();
      }

      auto& back()
      {
        return m_data.back();
      }
      const auto& back() const
      {
        return m_data.back();
      }

      bool empty() const
      {
        return length() == 0;
      }

      basic_string_view<CharType> to_view() const
      {
        return basic_string_view<CharType>(data(), length());
      }
      operator basic_string_view<CharType>() const // NOLINT(google-explicit-constructor)
      {
        return to_view();
      }

      void clear()
      {
        m_null_terminator_offset = 0;
      }
      void push_back(value_type c)
      {
        m_data[m_null_terminator_offset] = c;
        ++m_null_terminator_offset;
      }
      void pop_back()
      {
        --m_null_terminator_offset;
        m_data[m_null_terminator_offset] = CharType();
      }
      void resize(card32 size)
      {
        const card32 new_size = rsl::clamp_max(size, StrMaxSize - 1);
        const card32 old_size = length();

        for(card32 i = old_size; i < new_size; ++i)
        {
          m_data[i] = CharType();
        }

        m_null_terminator_offset = new_size;
      }
      // usually called after a memcpy call
      // can be solved be calling resize before the memcpy call
      void reset_null_termination_offset()
      {
        m_null_terminator_offset = rsl::string_length(data());
      }

      void remove_prefix(card32 count)
      {
        basic_string_view<CharType> view = to_view();
        view.remove_prefix(count);
        *this = view;
      }
      void remove_suffix(card32 count)
      {
        basic_string_view<CharType> view = to_view();
        view.remove_suffix(count);
        *this = view;
      }

      void copy(value_type* dest, card32 count, card32 pos = 0) const
      {
        basic_string_view<CharType> view = to_view();
        view.copy(dest, count, pos);
      }

      stack_string substr(card32 pos = 0, card32 count = npos()) const
      {
        basic_string_view<CharType> view   = to_view();
        basic_string_view<CharType> substr = view.substr(pos, count);
        return stack_string(substr);
      }

      template <card32 RhsSize>
      int32 compare(const stack_string<CharType, RhsSize>& rhs) const
      {
        basic_string_view<CharType> view     = to_view();
        basic_string_view<CharType> rhs_view = rhs.to_view();

        return view.compare(rhs_view);
      }
      template <card32 RhsSize>
      int32 compare(card32 pos1, card32 count1, const stack_string<CharType, RhsSize>& rhs, card32 pos2, card32 count2) const
      {
        basic_string_view<CharType> view     = to_view();
        basic_string_view<CharType> rhs_view = rhs.to_view();

        return view.compare(pos1, count1, rhs_view, pos2, count2);
      }
      int32 compare(const value_type* str) const
      {
        basic_string_view<CharType> view = to_view();
        return view.compare(str);
      }
      int32 compare(card32 pos1, card32 count1, const value_type* str) const
      {
        basic_string_view<CharType> view = to_view();
        return view.compare(pos1, count1, str);
      }
      int32 compare(card32 pos1, card32 count1, const value_type* str, card32 count2) const
      {
        basic_string_view<CharType> view = to_view();
        return view.compare(pos1, count1, str, count2);
      }

      constexpr void erase(card32 offset)
      {
        for(card32 i = offset; i < max_size() - 1; ++i)
        {
          m_data[i] = m_data[i + 1];
        }

        --m_null_terminator_offset;
      }
      void erase(card32 offset, card32 count)
      {
        for(card32 i = offset; i < max_size() - count; ++i)
        {
          m_data[i] = m_data[i + count];
        }

        m_null_terminator_offset -= count;
      }

      card32 find(basic_string_view<CharType> view, card32 pos = 0) const
      {
        basic_string_view<CharType> view = to_view();
        return view.find(view, pos);
      }
      card32 find(value_type c, card32 pos = 0) const
      {
        basic_string_view<CharType> view = to_view();
        return view.find(c, pos);
      }
      card32 find(const value_type* str, card32 pos, card32 count) const
      {
        basic_string_view<CharType> view = to_view();
        return view.find(str, pos, count);
      }
      card32 find(const value_type* str, card32 pos) const
      {
        basic_string_view<CharType> view = to_view();
        return view.find(str, pos);
      }

      card32 rfind(basic_string_view<CharType> view, card32 pos = npos()) const
      {
        basic_string_view<CharType> view = to_view();
        return view.rfind(view, pos);
      }
      card32 rfind(value_type c, card32 pos = npos()) const
      {
        basic_string_view<CharType> view = to_view();
        return view.rfind(c, pos);
      }
      card32 rfind(const value_type* str, card32 pos, card32 count) const
      {
        basic_string_view<CharType> view = to_view();
        return view.rfind(str, pos, count);
      }
      card32 rfind(const value_type* str, card32 pos = npos()) const
      {
        basic_string_view<CharType> view = to_view();
        return view.rfind(str, pos);
      }

      card32 find_first_of(basic_string_view<CharType> v, card32 pos = 0) const
      {
        basic_string_view<CharType> view = to_view();
        return view.find_first_of(v, pos);
      }
      card32 find_first_of(value_type c, card32 pos) const
      {
        basic_string_view<CharType> view = to_view();
        return view.find_first_of(c, pos);
      }
      card32 find_first_of(const value_type* s, card32 pos, card32 count) const
      {
        basic_string_view<CharType> view = to_view();
        return view.find_first_of(s, pos, count);
      }
      card32 find_first_of(const value_type* s, card32 pos = npos()) const
      {
        basic_string_view<CharType> view = to_view();
        return view.find_first_of(s, pos);
      }

      card32 find_last_of(basic_string_view<CharType> v, card32 pos = npos()) const
      {
        basic_string_view<CharType> view = to_view();
        return view.find_last_of(v, pos);
      }
      card32 find_last_of(value_type c, card32 pos = npos()) const
      {
        basic_string_view<CharType> view = to_view();
        return view.find_last_of(c, pos);
      }
      card32 find_last_of(const value_type* s, card32 pos, card32 count) const
      {
        basic_string_view<CharType> view = to_view();
        return view.find_last_of(s, pos, count);
      }
      card32 find_last_of(const value_type* s, card32 pos = npos()) const
      {
        basic_string_view<CharType> view = to_view();
        return view.find_last_of(s, pos);
      }

      card32 find_first_not_of(basic_string_view<CharType> v, card32 pos = 0) const
      {
        basic_string_view<CharType> view = to_view();
        return view.find_first_not_of(v, pos);
      }
      card32 find_first_not_of(value_type c, card32 pos = 0) const
      {
        basic_string_view<CharType> view = to_view();
        return view.find_first_not_of(c, pos);
      }
      card32 find_first_not_of(const value_type* s, card32 pos, card32 count) const
      {
        basic_string_view<CharType> view = to_view();
        return view.find_first_not_of(s, pos, count);
      }
      card32 find_first_not_of(const value_type* s, card32 pos = 0) const
      {
        basic_string_view<CharType> view = to_view();
        return view.find_first_not_of(s, pos);
      }

      card32 find_last_not_of(basic_string_view<CharType> v, card32 pos = 0) const
      {
        basic_string_view<CharType> view = to_view();
        return view.find_last_not_of(v, pos);
      }
      card32 find_last_not_of(value_type c, card32 pos = npos()) const
      {
        basic_string_view<CharType> view = to_view();
        return view.find_last_not_of(c, pos);
      }
      card32 find_last_not_of(const value_type* s, card32 pos, card32 count) const
      {
        basic_string_view<CharType> view = to_view();
        return view.find_last_not_of(s, pos, count);
      }
      card32 find_last_not_of(const value_type* s, card32 pos = 0) const
      {
        basic_string_view<CharType> view = to_view();
        return view.find_last_not_of(s, pos);
      }

      template <card32 Size>
      bool starts_with(const stack_string<CharType, Size>& prefix) const
      {
        basic_string_view<CharType> view = to_view();
        return view.starts_with(prefix.to_view());
      }
      bool starts_with(const value_type* prefix) const
      {
        basic_string_view<CharType> view = to_view();
        return view.starts_with(prefix);
      }
      bool starts_with(const basic_string_view<CharType> prefix) const
      {
        basic_string_view<CharType> view = to_view();
        return view.starts_with(prefix);
      }

      template <card32 Size>
      bool ends_with(const stack_string<CharType, Size>& suffix) const
      {
        basic_string_view<CharType> view = to_view();
        return view.ends_with(suffix.to_view());
      }
      bool ends_with(const value_type* suffix) const
      {
        basic_string_view<CharType> view = to_view();
        return view.ends_width(suffix);
      }
      bool ends_width(const basic_string_view<CharType> suffix) const
      {
        basic_string_view<CharType> view = to_view();
        return view.ends_width(suffix);
      }

      stack_string<CharType, StrMaxSize> to_lower() const
      {
        stack_string<CharType, StrMaxSize> lowered = *this;
        lowered.lower();
        return lower;
      }
      void lower()
      {
        rsl::to_lower(data(), data(), length());
      }
      stack_string<CharType, StrMaxSize> to_upper() const
      {
        stack_string<CharType, StrMaxSize> uppered = *this;
        uppered.upper();
        return uppered;
      }
      void upper()
      {
        rsl::to_upper(data(), data(), length());
      }

      bool is_letters() const
      {
        return rsl::is_letters(data(), length());
      }
      bool is_digits() const
      {
        return rsl::is_digits(data(), length());
      }
      bool is_digitsf() const
      {
        return rsl::is_digitsf(data(), length());
      }

      bool operator==(const basic_string_view<CharType> str) const
      {
        return to_view() == str;
      }
      bool operator!=(const basic_string_view<CharType> str) const
      {
        return !(*this == str);
      }

      stack_string operator+(const value_type c) const
      {
        stack_string res = *this;
        res.push_back(c);
        return res;
      }

      stack_string operator+(const basic_string_view<CharType>& str) const
      {
        stack_string res = *this;
        res += str;
        return res;
      }

      template <card32 RhsSize>
      auto operator+(const stack_string<CharType, RhsSize>& str) const
      {
        stack_string<CharType, (rsl::max)(StrMaxSize, RhsSize)> result;

        result += *this;
        result += str;

        return result;
      }

      stack_string& operator+=(value_type c)
      {
        push_back(c);
        return *this;
      }

      stack_string& operator+=(const value_type* str)
      {
        return operator+=(basic_string_view<CharType>(str));
      }

      stack_string& operator+=(const basic_string_view<CharType> str)
      {
        const card32 string_length   = str.length();
        const card32 remaining_bytes = rsl::clamp_max(string_length, StrMaxSize);

        append(str.data(), remaining_bytes);
        return *this;
      }

      void append(const value_type* str, card32 count)
      {
        REX_ASSERT_X(length() + count <= max_size(), "copying into a stack string beyond its limit");

        value_type* dest = data();
        dest += length();
        rsl::memcpy(dest, str, count);
        m_null_terminator_offset += count;
      }

      static card32 npos()
      {
        return s_npos;
      }

      bool operator<(const stack_string<CharType, StrMaxSize>& str) const
      {
        return compare(str.data(), str.length()) < 0;
      }
      bool operator<=(const stack_string<CharType, StrMaxSize>& str) const
      {
        return compare(str.data(), str.length()) <= 0;
      }
      bool operator>(const stack_string<CharType, StrMaxSize>& str) const
      {
        return compare(str.data(), str.length()) > 0;
      }
      bool operator>=(const stack_string<CharType, StrMaxSize>& str) const
      {
        return compare(str.data(), str.length()) >= 0;
      }

    private:
      rsl::array<CharType, StrMaxSize> m_data;
      card32 m_null_terminator_offset; // NOLINT(misc-non-private-member-variables-in-classes): ignoring false positive

      constexpr static card32 s_npos = -1;
    };

    template <typename CharType, card32 Size>
    stack_string<CharType, Size> operator+(const CharType* str, const stack_string<CharType, Size>& string)
    {
      stack_string<CharType, Size> res(str);
      res += string;
      return res;
    }

    using tiny_stack_string   = stack_string<char8, 32>;  // NOLINT(readability-magic-numbers)
    using small_stack_string  = stack_string<char8, 64>;  // NOLINT(readability-magic-numbers)
    using medium_stack_string = stack_string<char8, 128>; // NOLINT(readability-magic-numbers)
    using big_stack_string    = stack_string<char8, 256>; // NOLINT(readability-magic-numbers)

    tiny_stack_string to_stack_string(uint32 value);
    tiny_stack_string to_stack_string(int32 value);
    tiny_stack_string to_stack_string(uint64 value);
    tiny_stack_string to_stack_string(int64 value);
    tiny_stack_string to_stack_string(float32 value, card32 precision = 4);
    tiny_stack_string to_stack_string(float64 value, card32 precision = 4);
    tiny_stack_string to_stack_string(const void* ptr);

    template <typename CharType, typename Traits>
    class basic_ostream;

    template <typename CharT, typename Traits>
    class basic_streambuf;

    template <typename CharType, typename Traits, card32 StrMaxSize>
    basic_ostream<CharType, Traits>& operator<<(basic_ostream<CharType, Traits>& os, const stack_string<CharType, StrMaxSize>& str)
    {
      os.rdbuf()->sputn(str.data(), str.length());
      return os;
    }

  } // namespace v1
} // namespace rsl

rsl::tiny_stack_string operator"" _tiny(const char* str, size_t length);
rsl::small_stack_string operator"" _small(const char* str, size_t length);
rsl::medium_stack_string operator"" _med(const char* str, size_t length);
rsl::big_stack_string operator"" _big(const char* str, size_t length);

namespace rsl
{
  inline namespace v1
  {

    template <typename CharType, card32 Size>
    struct hash<rsl::stack_string<CharType, Size>>
    {
      card32 operator()(const rsl::stack_string<CharType, Size>& str) const
      {
        return rsl::hash(str);
      }
    };

  } // namespace v1
} // namespace rsl
