#pragma once

#include "rex_std/string.h"

namespace rex
{
  class StringID;

  namespace string_pool
  {
    StringID find_or_store(rsl::string_view string);
  } // namespace string_pool
} // namespace rex