#pragma once

#include "rex_std/string_view.h"
#include "rex_engine/engine/types.h"
#include "rex_engine/memory/memory_types.h"

namespace rex
{
  // Returns the endline characters for the current platform
  rsl::string_view endline();

  // Removes leading and trailing whitespace
  rsl::string_view strip(rsl::string_view input);

  // Removes leading and trailing characters that match any character in the view
  rsl::string_view strip(rsl::string_view input, rsl::string_view characters);

  // Removes leading whitespace
  rsl::string_view lstrip(rsl::string_view input);

  // Removes leading characters that match any character in the view
  rsl::string_view lstrip(rsl::string_view input, rsl::string_view characters);

  // Removes trailing whitespace
  rsl::string_view rstrip(rsl::string_view input);

  // Removes trailing characters that match any character in the view
  rsl::string_view rstrip(rsl::string_view input, rsl::string_view characters);

  // Add quotes around a string
  scratch_string quoted(rsl::string_view input);

  // Removes all spaces from a string
  // we're taking a string by ref
  // to reuse the memory
  template <typename Allocator>
  rsl::basic_string<char8, rsl::char_traits<char8>, Allocator>& remove_spaces(rsl::basic_string<char8, rsl::char_traits<char8>, Allocator>& input)
  {
    input.replace(" ", "");
    return input;
  }

  // removes leading and trailing quotes from a path
  rsl::string_view remove_quotes(rsl::string_view path);

  // converts a wide character string to a multi byte character string
  rsl::string to_multibyte(const tchar* wideCharacterBuffer, count_t length);

  // returns true if the character after the string view is '\0'
  bool is_null_terminated(rsl::string_view string);

  // returns the shortname of the day presented by the nr.
  // according to the international standard ISO 8601
  // https://en.wikipedia.org/wiki/Names_of_the_days_of_the_week#:~:text=In%20the%20international%20standard%20ISO,second%20day%20of%20the%20week.
  // Monday starts at index 1
  rsl::string_view day_nr_to_name(s32 dayNr);

  // returns the full name of the day presented by the nr.
  // according to the international standard ISO 8601
  // https://en.wikipedia.org/wiki/Names_of_the_days_of_the_week#:~:text=In%20the%20international%20standard%20ISO,second%20day%20of%20the%20week.
  // Monday starts at index 1
  rsl::string_view day_nr_to_full_name(s32 dayNr);

  // returns the numeral representation of a day
  // according to the international standard ISO 8601
  // https://en.wikipedia.org/wiki/Names_of_the_days_of_the_week#:~:text=In%20the%20international%20standard%20ISO,second%20day%20of%20the%20week.
  // Monday starts at index 1
  s32 day_name_to_nr(rsl::string_view dayName);

  // returns the shortname of the month presented by the nr.
  // January starts at index 1
  rsl::string_view month_nr_to_name(s32 monthNr);

  // returns the full name of the month presented by the nr.
  // January starts at index 1
  rsl::string_view month_nr_to_full_name(s32 monthNr);

  // returns the numeral representation of a month
  // January starts at index 1
  s32 month_name_to_nr(rsl::string_view monthName);

  // converts a multi byte string to a wide character string
  template <typename StringType>
  StringType to_wstring(rsl::string_view str)
  {
    StringType wide_str;
    wide_str.resize(str.size());
    to_wstring(str, wide_str.data(), wide_str.length() + 1);

    return wide_str;
  }
  void to_wstring(rsl::string_view str, tchar* outBuffer, s32 maxLength);

  // converts a wide character string to a multi byte string
  template <typename StringType>
  StringType to_string(rsl::wstring_view wideStr)
  {
    StringType str;
    str.resize(wideStr.size());
    to_string(wideStr, str.data(), str.length() + 1);

    return str;
  }
  void to_string(rsl::wstring_view str, char8* outBuffer, s32 maxLength);


} // namespace rex