#pragma once

#include "rex_std/string.h"

namespace rex
{
  class StringEntry;
  class StringEntryID;

  namespace string_pool
  {
    rsl::string_view resolve(const StringEntryID& entryID);

    const StringEntry* find(const StringEntryID& entryID);

    StringEntryID make_and_store(rsl::string_view characters);
  } // namespace string_pool
} // namespace rex