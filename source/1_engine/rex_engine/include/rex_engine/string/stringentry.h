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

  class StringEntryID
  {
  public:
    static StringEntryID create_invalid();

    StringEntryID();
    StringEntryID(rsl::hash_result value); // NOLINT(google-explicit-constructor)

    bool operator<(const StringEntryID& rhs) const;
    bool operator>(const StringEntryID& rhs) const;

    bool operator==(const StringEntryID& rhs) const;
    bool operator==(const rsl::hash_result& rhs) const;
    bool operator!=(const StringEntryID& rhs) const;
    bool operator!=(const rsl::hash_result& rhs) const;

    explicit operator bool() const;
    explicit operator rsl::hash_result() const;

  private:
    rsl::hash_result m_value;
  };
} // namespace rex

namespace rsl
{
  inline namespace v1
  {
    template <>
    struct hash<rex::StringEntryID>
    {
      rsl::hash_result operator()(const rex::StringEntryID& k) const
      {
        return static_cast<rsl::hash_result>(k);
      }
    };
  } // namespace v1
} // namespace rsl