#include "rex_engine/text_processing/text_processing.h"

#include "rex_std/algorithm.h"
#include "rex_std/ctype.h"

namespace rex
{
  rsl::string_view endline()
  {
#ifdef REX_PLATFORM_WINDOWS
    return "\n\r";
#else
    return "\n";
#endif
  }

  // Removes leading and trailing whitespace
  rsl::string_view strip(rsl::string_view input)
  {
    // clang-format off
    auto first_not_whitespace = rsl::find_if(input.cbegin(), input.cend(),
      [](const char8 c)
      {
        return !rsl::is_space(c);
      });

    auto last_not_whitespace = rsl::find_if(input.crbegin(), input.crend(),
      [](const char8 c)
      {
        return !rsl::is_space(c);
      }).base();
    // clang-format on

    if (last_not_whitespace > first_not_whitespace)
    {
      return rsl::string_view(first_not_whitespace, last_not_whitespace);
    }

    return "";
  }

  // Removes leading and trailing characters that match any character in the view
  rsl::string_view strip(rsl::string_view input, rsl::string_view characters)
  {
    auto first_not_character = rsl::find_first_not_of(input.cbegin(), input.cend(), characters.cbegin(), characters.cend());
    auto last_not_character  = rsl::find_last_not_of(input.cbegin(), input.cend(), characters.cbegin(), characters.cend());

    if (last_not_character > first_not_character)
    {
      return rsl::string_view(first_not_character, last_not_character + 1);
    }

    return "";
  }

  // Removes leading whitespace
  rsl::string_view lstrip(rsl::string_view input)
  {
    // clang-format off
    auto first_not_whitespace = rsl::find_if(input.cbegin(), input.cend(),
      [](const char8 c)
      {
        return !rsl::is_space(c);
      });
    // clang-format on

    return rsl::string_view(first_not_whitespace, input.cend());
  }

  // Removes leading characters that match any character in the view
  rsl::string_view lstrip(rsl::string_view input, rsl::string_view characters)
  {
    auto first_not_whitespace = rsl::find_first_not_of(input.cbegin(), input.cend(), characters.cbegin(), characters.cend());

    return rsl::string_view(first_not_whitespace, input.cend());
  }

  // Removes trailing whitespace
  rsl::string_view rstrip(rsl::string_view input)
  {
    // clang-format off
    auto last_not_whitespace = rsl::find_if(input.crbegin(), input.crend(),
      [](const char8 c)
      {
        return !rsl::is_space(c);
      }).base();
    // clang-format on

    return rsl::string_view(input.cbegin(), last_not_whitespace);
  }

  // Removes trailing characters that match any character in the view
  rsl::string_view rstrip(rsl::string_view input, rsl::string_view characters)
  {
    auto last_not_whitespace = rsl::find_last_not_of(input.cbegin(), input.cend(), characters.cbegin(), characters.cend());

    return rsl::string_view(input.cbegin(), last_not_whitespace + 1);
  }

  // Add quotes around a string
  rsl::string quoted(rsl::string_view input)
  {
    if (input.starts_with('"') && input.ends_with('"'))
    {
      return rsl::string(input);
    }

    rsl::string res;
    res.reserve(input.size() + 2);

    res += "\"";
    res += input;
    res += "\"";

    return res;
  }

  // Removes all spaces from a string
  // we're taking a string by ref
  // to reuse the memory
  rsl::string& remove_spaces(rsl::string& input)
  {
    input.replace(" ", "");
    return input;
  }

  // removes leading and trailing quotes from a path
  rsl::string_view remove_quotes(rsl::string_view path)
  {
    if (path.starts_with("\"") && path.ends_with("\""))
    {
      path = path.substr(1, path.length() - 2);
    }

    return path;
  }

  // Converst a wide character string to a multi byte character string
  rsl::string to_multibyte(const tchar* wideCharacterBuffer, count_t size)
  {
    rsl::string buffer;
    buffer.resize(size);

    // Convert wide character string to multi byte character string.
    // size_t converted_chars => The amount of converted characters.
    // 0 terminate the string afterwards.
    size_t converted_chars = 0;
    auto result = wcstombs_s(&converted_chars, buffer.data(), size, wideCharacterBuffer, size);
    if (result != 0)
    {
      return rsl::string("Error converting wide string to multi byte string");
    }

    // the null char also acts as a converted char, 
    // so we need to make sure we decrement the number of converted chars
    // so we don't end up with an invalid length
    return rsl::string(buffer.data(), static_cast<count_t>(converted_chars - 1)); // NOLINT(readability-redundant-string-cstr)
  }

  // returns true if the character after the string view is '\0'
  bool is_null_terminated(rsl::string_view string)
  {
    return string[string.length()] == '\0';
  }

  // returns the shortname of the day presented by the nr.
  // according to the international standard ISO 8601
  // https://en.wikipedia.org/wiki/Names_of_the_days_of_the_week#:~:text=In%20the%20international%20standard%20ISO,second%20day%20of%20the%20week.
  // Monday starts at index 1
  rsl::string_view day_nr_to_name(s32 dayNr)
  {
    switch (dayNr)
    {
    case 1:
      return "Mon";
    case 2:
      return "Tue";
    case 3:
      return "Wed";
    case 4:
      return "Thu";
    case 5:
      return "Fri";
    case 6:
      return "Sat";
    case 7:
      return "Sun";
    default:
      return "Invalid input";
    }
  }

  // returns the full name of the day presented by the nr.
  // according to the international standard ISO 8601
  // https://en.wikipedia.org/wiki/Names_of_the_days_of_the_week#:~:text=In%20the%20international%20standard%20ISO,second%20day%20of%20the%20week.
  // Monday starts at index 1
  rsl::string_view day_nr_to_full_name(s32 dayNr)
  {
    switch (dayNr)
    {
    case 1:
      return "Monday";
    case 2:
      return "Tuesday";
    case 3:
      return "Wednesday";
    case 4:
      return "Thursday";
    case 5:
      return "Friday";
    case 6:
      return "Saturday";
    case 7:
      return "Sunday";
    default:
      return "Invalid input";
    }
  }

  // returns the numeral representation of a day
  // according to the international standard ISO 8601
  // https://en.wikipedia.org/wiki/Names_of_the_days_of_the_week#:~:text=In%20the%20international%20standard%20ISO,second%20day%20of%20the%20week.
  // Monday starts at index 1
  s32 day_name_to_nr(rsl::string_view dayName)
  {
    rsl::small_stack_string day_name(dayName);
    day_name.lower();

    if (day_name == "monday" || day_name == "mon") return 1;
    if (day_name == "tuesday" || day_name == "tue") return 2;
    if (day_name == "wednesday" || day_name == "wed") return 3;
    if (day_name == "thursday" || day_name == "thu") return 4;
    if (day_name == "friday" || day_name == "fri") return 5;
    if (day_name == "saturday" || day_name == "sat") return 6;
    if (day_name == "sunday" || day_name == "sun") return 7;

    return -1; // Invalid input
  }

  // returns the shortname of the month presented by the nr.
  // January starts at index 1
  rsl::string_view month_nr_to_name(s32 monthNr)
  {
    switch (monthNr)
    {
    case 1:
      return "Jan";
    case 2:
      return "Feb";
    case 3:
      return "Mar";
    case 4:
      return "Apr";
    case 5:
      return "May";
    case 6:
      return "Jun";
    case 7:
      return "Jul";
    case 8:
      return "Aug";
    case 9:
      return "Sep";
    case 10:
      return "Oct";
    case 11:
      return "Nov";
    case 12:
      return "Dec";
    default:
      return "Invalid input";
    }
  }

  // returns the full name of the month presented by the nr.
  // January starts at index 1
  rsl::string_view month_nr_to_full_name(s32 monthNr)
  {
    switch (monthNr)
    {
    case 1:
      return "January";
    case 2:
      return "February";
    case 3:
      return "March";
    case 4:
      return "April";
    case 5:
      return "May";
    case 6:
      return "June";
    case 7:
      return "July";
    case 8:
      return "August";
    case 9:
      return "September";
    case 10:
      return "October";
    case 11:
      return "November";
    case 12:
      return "December";
    default:
      return "Invalid input";
    }
  }

  // returns the numeral representation of a month
  // January starts at index 1
  s32 month_name_to_nr(rsl::string_view monthName)
  {
    if (monthName == "january" || monthName == "jan") return 1;
    if (monthName == "february" || monthName == "feb") return 2;
    if (monthName == "march" || monthName == "mar") return 3;
    if (monthName == "april" || monthName == "apr") return 4;
    if (monthName == "may") return 5;
    if (monthName == "june" || monthName == "jun") return 6;
    if (monthName == "july" || monthName == "jul") return 7;
    if (monthName == "august" || monthName == "aug") return 8;
    if (monthName == "september" || monthName == "sep") return 9;
    if (monthName == "october" || monthName == "oct") return 10;
    if (monthName == "november" || monthName == "nov") return 11;
    if (monthName == "december" || monthName == "dec") return 12;

    return -1; // Invalid input
  }

} // namespace rex