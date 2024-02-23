#pragma once

#include "rex_std/string_view.h"

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

  // Removes all spaces from a string
  // we're taking a string by ref
  // to reuse the memory
  void remove_spaces(rsl::string& input);
}