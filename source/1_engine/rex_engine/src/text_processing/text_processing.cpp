#include "rex_engine/text_processing/text_processing.h"

#include "rex_std/ctype.h"
#include "rex_std/algorithm.h"

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

      return rsl::string_view(first_not_whitespace, last_not_whitespace);
  }

  // Removes leading and trailing characters that match any character in the view
  rsl::string_view strip(rsl::string_view input, rsl::string_view characters)
  {
    auto first_not_whitespace = rsl::find_first_not_of(input.cbegin(), input.cend(), characters.cbegin(), characters.cend());
    auto last_not_whitespace = rsl::find_last_not_of(input.cbegin(), input.cend(), characters.cbegin(), characters.cend());

    return rsl::string_view(first_not_whitespace, last_not_whitespace);
  }

  // Removes leading whitespace
  rsl::string_view lstrip(rsl::string_view input)
  {
    auto first_not_whitespace = rsl::find_if(input.cbegin(), input.cend(),
      [](const char8 c)
      {
        return !rsl::is_space(c);
      });

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
    auto last_not_whitespace = rsl::find_if(input.crbegin(), input.crend(),
      [](const char8 c)
      {
        return !rsl::is_space(c);
      }).base();

      return rsl::string_view(input.cbegin(), last_not_whitespace);
  }

  // Removes trailing characters that match any character in the view
  rsl::string_view rstrip(rsl::string_view input, rsl::string_view characters)
  {
    auto last_not_whitespace = rsl::find_last_not_of(input.cbegin(), input.cend(), characters.cbegin(), characters.cend());

    return rsl::string_view(input.cbegin(), last_not_whitespace);
  }

  // Removes all spaces from a string
  // we're taking a string by ref
  // to reuse the memory
  void remove_spaces(rsl::string& input)
  {
    input.replace(" ", "");
  }
}