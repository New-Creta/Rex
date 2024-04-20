#pragma once

#include "rex_engine/engine/types.h"
#include "rex_std/bonus/functional.h"
#include "rex_std/bonus/types.h"
#include "rex_std/format.h"
#include "rex_std/internal/format/core.h"
#include "rex_std/ostream.h"

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
     * Create an invalid StringID.
     */
    static constexpr StringID create_invalid()
    {
      return StringID{};
    }

    //-------------------------------------------------------------------------
    /**
     * Create a string id object and store it in the string pool
     */
    static StringID create(rsl::string_view string);

    //-------------------------------------------------------------------------
    /**
     * Create an None StringID.
     */
    constexpr StringID()
      : m_comparison_hash(s_none_state_hash_val)
      , m_string("Invalid StringID")
    {
    }

    //-------------------------------------------------------------------------
    /**
     * Create a runtime generated StringID.
     * Users are not expected to call this directly,
     * instead call the static "create" function which
     * which will create a string id and intern it in the string pool
     */
    constexpr StringID(rsl::hash_result entryID, rsl::string_view str)
      : m_comparison_hash(entryID)
      , m_string(str)
    {
    }

    //-------------------------------------------------------------------------
    /**
     * Retrieve the hashed value as u32
     */
    constexpr operator u32() const // NOLINT(google-explicit-constructor)
    {
      return static_cast<u32>(m_comparison_hash);
    }
    //-------------------------------------------------------------------------
    /**
     * Is the hash value valid
     */
    constexpr explicit operator bool() const
    {
      return is_valid();
    }

    //-------------------------------------------------------------------------
    constexpr bool operator<(const StringID& rhs) const
    {
      return m_comparison_hash < rhs.m_comparison_hash;
    }
    //-------------------------------------------------------------------------
    constexpr bool operator>(const StringID& rhs) const
    {
      return rhs.m_comparison_hash < m_comparison_hash;
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
    /** True for StringID() and StringID("Invalid StringID") */
    constexpr bool is_valid() const
    {
      return m_comparison_hash != s_none_state_hash_val;
    }

    //-------------------------------------------------------------------------
    /**
     * Retrieve the hashed value
     */
    constexpr u32 value() const
    {
      return static_cast<u32>(m_comparison_hash);
    }

    //-------------------------------------------------------------------------
    // Return the underlying string
    constexpr rsl::string_view string() const
    {
      return m_string;
    }

    //-------------------------------------------------------------------------
    // Return the length of the underlying string
    constexpr s32 length() const
    {
      return m_string.length();
    }

  private:
    /** Hash for invalid StringID into string pool */
    static constexpr u32 s_none_state_hash_val = 0;

    /** Hash into the StringID hash table */
    rsl::hash_result m_comparison_hash;

    // view to the underlying string that the string id represents
    rsl::string_view m_string;
  };

} // namespace rex

// create a compile time string id but it doesn't get stored in the string pool
constexpr rex::StringID operator""_sid(const char* string, size_t size)
{
  rsl::string_view view(string, static_cast<u32>(size)); // NOLINT(cppcoreguidelines-narrowing-conversions)
  rsl::hash_result hash = rsl::hash<rsl::string_view>{}(view);
  return rex::StringID(hash, view);
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

    //-------------------------------------------------------------------------
    template <>
    struct rsl::formatter<rex::StringID>
    {
      constexpr auto parse(format_parse_context& ctx) // NOLINT (readability-convert-member-functions-to-static)
      {
        return ctx.begin();
      }

      template <typename FormatContext>
      auto format(const rex::StringID& sid, FormatContext& ctx)
      {
        // Format your type's output here
        return rsl::format_to(ctx.out(), "{}", rex::restore_sid(sid));
      }
    };
  } // namespace v1
} // namespace rsl