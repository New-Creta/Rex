#pragma once

#include "rex_std/string_view.h"

namespace rex::win
{
  int32 message_box(rsl::string_view msg);
  int32 message_box(rsl::string_view msg, rsl::string_view title);

  // Returns true if yes is pressed
  bool yes_no_message_box(rsl::string_view msg);
  bool yes_no_message_box(rsl::string_view msg, rsl::string_view title);
} // namespace rex::win