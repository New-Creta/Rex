#pragma once

#include "rex_engine/string/stringentry.h"
#include "rex_engine/types.h"
#include "rex_std/ostream.h"
#include "rex_std/string.h"

namespace rex
{
  class StringID
  {
  public:
    StringID();
    explicit StringID(rsl::string_view characters);

    explicit operator u32() const;

    bool operator==(const StringID& other) const;
    bool operator!=(const StringID& other) const;

    bool operator==(const StringEntryID& entryID) const;
    bool operator!=(const StringEntryID& entryID) const;

    rsl::string_view to_string_view() const;
    bool is_none() const;
    u32 value() const;

  private:
    /** Hash into the StringID hash table */
    StringEntryID m_comparison_hash;
  };

  StringID create_sid(rsl::string_view stringView);

  bool operator==(rsl::string_view s, const StringID& sid);
  bool operator!=(rsl::string_view s, const StringID& sid);
  bool operator==(const StringID& sid, rsl::string_view s);
  bool operator!=(const StringID& sid, rsl::string_view s);
} // namespace rex

rex::StringID operator""_sid(const char* string, size_t size);

rsl::ostream& operator<<(rsl::ostream& os, const rex::StringID& stringID);

// custom specialization of rsl::hash can be injected in namespace rsl
namespace rsl
{
  inline namespace v1
  {
    //-------------------------------------------------------------------------
    template <>
    struct hash<rex::StringID>
    {
      rsl::hash_result operator()(const rex::StringID& s) const noexcept
      {
        return s.value();
      }
    };
  } // namespace v1
} // namespace rsl