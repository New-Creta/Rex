// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: basic_string.cpp
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#include "rex_std/internal/string/basic_string.h"

namespace rsl
{
  inline namespace v1
  {
    rsl::optional<int32> stoi(const string& str, char8* strEnd, int32 base)
    {
      return rsl::strtol(str.data(), &strEnd, base);
    }
    rsl::optional<long> stol(const string& str, char8* strEnd, int32 base)
    {
      return rsl::strtol(str.data(), &strEnd, base);
    }
    rsl::optional<int64> stoll(const string& str, char8* strEnd, int32 base)
    {
      return rsl::strtoll(str.data(), &strEnd, base);
    }
    rsl::optional<uint32> stoui(const string& str, char8* strEnd, int32 base)
    {
      return rsl::strtoul(str.data(), &strEnd, base);
    }
    rsl::optional<ulong> stoul(const string& str, char8* strEnd, int32 base)
    {
      return rsl::strtoul(str.data(), &strEnd, base);
    }
    rsl::optional<uint64> stoull(const string& str, char8* strEnd, int32 base)
    {
      return rsl::strtoull(str.data(), &strEnd, base);
    }
    rsl::optional<float32> stof(const string& str, char8* strEnd)
    {
      return rsl::strtof(str.data(), &strEnd);
    }
    rsl::optional<float64> stod(const string& str, char8* strEnd)
    {
      return rsl::strtod(str.data(), &strEnd);
    }
    rsl::optional<lfloat64> stold(const string& str, char8* strEnd)
    {
      return rsl::strtold(str.data(), &strEnd);
    }

    string to_string(int32 value)
    {
      return internal::to_string<string>(value);
    }
    string to_string(long value)
    {
      return internal::to_string<string>(value);
    }
    string to_string(int64 value)
    {
      return internal::to_string<string>(value);
    }
    string to_string(uint32 value)
    {
      return internal::to_string<string>(value);
    }
    string to_string(ulong value)
    {
      return internal::to_string<string>(value);
    }
    string to_string(uint64 value)
    {
      return internal::to_string<string>(value);
    }
    string to_string(float32 value)
    {
      return internal::to_string<string>(value);
    }
    string to_string(float64 value)
    {
      return internal::to_string<string>(value);
    }
    string to_string(lfloat64 value)
    {
      return internal::to_string<string>(value);
    }

    namespace string_literals
    {
#if defined(REX_MSVC_COMPILER)
  #pragma warning(push)
  #pragma warning(disable : 4455) // literal suffix identifiers that do not start with an underscore are reserved
#elif defined(REX_CLANG_COMPILER)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wuser-defined-literals" // literal suffix identifiers that do not start with an underscore are reserved
#endif
      // returns a string of the desired type
      string operator""s(const char8* s, size_t len) // NOLINT(clang-diagnostic-user-defined-literals)
      {
        const count_t len_as_count = static_cast<count_t>(len);
        return string(s, len_as_count);
      }
      // returns a string of the desired type
      u16string operator""s(const char16_t* s, size_t len) // NOLINT(clang-diagnostic-user-defined-literals)
      {
        const count_t len_as_count = static_cast<count_t>(len);
        return u16string(s, len_as_count);
      }
      // returns a string of the desired type
      u32string operator""s(const char32_t* s, size_t len) // NOLINT(clang-diagnostic-user-defined-literals)
      {
        const count_t len_as_count = static_cast<count_t>(len);
        return u32string(s, len_as_count);
      }
      // returns a string of the desired type
      wstring operator""s(const tchar* s, size_t len) // NOLINT(clang-diagnostic-user-defined-literals)
      {
        const count_t len_as_count = static_cast<count_t>(len);
        return wstring(s, len_as_count);
      }
#if defined(REX_MSVC_COMPILER)
  #pragma warning(pop)
#elif defined(REX_CLANG_COMPILER)
  #pragma clang diagnostic pop
#endif
    } // namespace string_literals
  }   // namespace v1
} // namespace rsl