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

    rsl::string_view characters() const;

    s32 size() const;

  private:
    rsl::unique_array<char> m_characters;
  };

  class StringEntryID
  {
  public:
    static StringEntryID create_invalid();

    //-------------------------------------------------------------------------
    constexpr StringEntryID()
        : m_value(s_none_state_hash)
    {
    }
    //-------------------------------------------------------------------------
    constexpr explicit StringEntryID(rsl::hash_result value)
        : m_value(value)
    {
    }

    //-------------------------------------------------------------------------
    constexpr bool operator<(const StringEntryID& rhs) const
    {
      return m_value < rhs.m_value;
    }
    //-------------------------------------------------------------------------
    constexpr bool operator>(const StringEntryID& rhs) const
    {
      return rhs.m_value < m_value;
    }

    //-------------------------------------------------------------------------
    constexpr bool operator==(const StringEntryID& rhs) const
    {
      return m_value == rhs.m_value;
    }
    //-------------------------------------------------------------------------
    constexpr bool operator==(const rsl::hash_result& rhs) const
    {
      return m_value == rhs;
    }
    //-------------------------------------------------------------------------
    constexpr bool operator!=(const StringEntryID& rhs) const
    {
      return m_value != rhs.m_value;
    }
    //-------------------------------------------------------------------------
    constexpr bool operator!=(const rsl::hash_result& rhs) const
    {
      return m_value != rhs;
    }
    //-------------------------------------------------------------------------
    constexpr explicit operator bool() const
    {
      return m_value != s_none_state_hash;
    }
    //-------------------------------------------------------------------------
    constexpr explicit operator rsl::hash_result() const
    {
      return m_value;
    }

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