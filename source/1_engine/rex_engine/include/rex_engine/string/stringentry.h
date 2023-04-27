#pragma once

#include "rex_engine/types.h"
#include "rex_std/functional.h"
#include "rex_std/memory.h"

namespace rex
{
  class StringEntry
  {
  public:
    StringEntry(const char* chars, s32 charCount) noexcept;
    StringEntry(const StringEntry& other) = delete;
    StringEntry(StringEntry&& other) noexcept;
    ~StringEntry();

    StringEntry& operator=(const StringEntry& other) = delete;
    StringEntry& operator=(StringEntry&& other) noexcept;

    void get_characters(const char** characters, s32& characterCount) const;
    const char* get_characters() const;

    s32 get_size() const;

  private:
    rsl::unique_ptr<char> m_characters;
    s32 m_size;
  };

  class StringEntryID
  {
  public:
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