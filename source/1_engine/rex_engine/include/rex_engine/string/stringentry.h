#pragma once

#include "rex_engine/types.h"
#include "rex_std/bonus/functional.h"
#include "rex_std/bonus/memory.h"
#include "rex_std/memory.h"
#include "rex_std/string.h"

namespace rex
{
  class StringEntry
  {
  public:
    StringEntry(rsl::string_view characters) noexcept; // NOLINT(google-explicit-constructor)
    StringEntry(const StringEntry& other) = delete;
    StringEntry(StringEntry&& other) noexcept;
    ~StringEntry();

    StringEntry& operator=(const StringEntry& other) = delete;
    StringEntry& operator=(StringEntry&& other) noexcept;

    bool is_valid() const;

    rsl::string_view characters() const;

    s32 size() const;

  private:
    rsl::unique_array<char> m_characters;
  };
} // namespace rex