#pragma once

#include "rex_std/string.h"

namespace rex
{
  class StringEntry;
  class StringID;

  namespace string_pool
  {
    rsl::string_view resolve(const StringID& entryID);

    const StringEntry& find(const StringID& entryID);

    StringID make_and_store(rsl::string_view characters);
  } // namespace string_pool
} // namespace rex