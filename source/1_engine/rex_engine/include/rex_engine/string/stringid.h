#pragma once

#include "rex_engine/string/stringentry.h"
#include "rex_engine/string/stringpool.h"
#include "rex_engine/types.h"
#include "rex_std/bonus/functional.h"
#include "rex_std/memory.h"
#include "rex_std/ostream.h"
#include "rex_std/string.h"

#include <cstddef>

namespace rex
{
  /**
   * The process of generating a standalone StringID object lacks a hash collision check,
   *   which implies that in extremely rare scenarios, different strings may exist with the same hash value.
   *
   * With CRC-32, which generates a 32-bit hash value, the theoretical probability of a collision occurring randomly is approximately 1 in 4.3 billion.
   * However, the likelihood can vary based on the data being hashed.
   *
   * If issues arise when using StringIDs, one possible explanation could be a hash collision.
   *
   * Hash collisions can only be prevented through the utilization of runtime generated StringIDs,
   *   which is achieved by employing the "rex::store_sid" function.
   */
  class StringID
  {
  public:
    //-------------------------------------------------------------------------
    /**
     * Create an empty StringID.
     */
    constexpr StringID()
        : m_comparison_hash(StringEntryID::s_none_state_hash)
    {
    }

    //-------------------------------------------------------------------------
    /**
     * Create a runtime generated StringEntryID.
     */
    constexpr explicit StringID(StringEntryID entryID)
        : m_comparison_hash(entryID)
    {
    }

    //-------------------------------------------------------------------------
    /**
     * Create an StringID with characters.
     */
    constexpr explicit StringID(rsl::string_view stringView)
        : m_comparison_hash(StringEntryID(rsl::hash<rsl::string_view> {}(stringView)))
    {
    }

    //-------------------------------------------------------------------------
    /**
     * Retrieve the hashed value
     */
    constexpr operator u32() const //NOLINT(google-explicit-constructor)
    {
      return static_cast<u32>(m_comparison_hash);
    }

    //-------------------------------------------------------------------------
    constexpr bool operator==(const StringID& other) const
    {
      return m_comparison_hash == other.m_comparison_hash;
    }
    //-------------------------------------------------------------------------
    constexpr bool operator!=(const StringID& other) const
    {
      return !(*this == other);
    }

    //-------------------------------------------------------------------------
    constexpr bool operator==(const StringEntryID& entryID) const
    {
      return m_comparison_hash == static_cast<uint32>(entryID);
    }
    //-------------------------------------------------------------------------
    constexpr bool operator!=(const StringEntryID& entryID) const
    {
      return m_comparison_hash != static_cast<uint32>(entryID);
    }

    //-------------------------------------------------------------------------
    /** True for StringID() and StringID("Invalid StringID") */
    constexpr bool is_none() const
    {
      return m_comparison_hash == StringEntryID::s_none_state_hash;
    }

    //-------------------------------------------------------------------------
    /**
     * Retrieve the hashed value
     */
    constexpr u32 value() const
    {
      return static_cast<u32>(m_comparison_hash);
    }

  private:
    /** Hash into the StringID hash table */
    StringEntryID m_comparison_hash;
  };

  StringID store_sid(rsl::string_view characters);

  bool operator==(rsl::string_view s, const StringID& sid);
  bool operator!=(rsl::string_view s, const StringID& sid);
  bool operator==(const StringID& sid, rsl::string_view s);
  bool operator!=(const StringID& sid, rsl::string_view s);
} // namespace rex

constexpr rex::StringID operator""_sid(const char* string, size_t size)
{
  return rex::StringID(rsl::string_view(string, static_cast<u32>(size))); // NOLINT(cppcoreguidelines-narrowing-conversions)
}

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