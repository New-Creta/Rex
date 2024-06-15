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

    return rsl::string(buffer.data(), static_cast<count_t>(converted_chars)); // NOLINT(readability-redundant-string-cstr)
  }
} // namespace rex