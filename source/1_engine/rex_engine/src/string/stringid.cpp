#include "rex_engine/string/stringid.h"

#include "rex_engine/string/stringpool.h"
#include "rex_engine/log.h"
#include "rex_std/memory.h"
#include "rex_std/vector.h"

namespace rex
{
  namespace internal
  {
    //-------------------------------------------------------------------------
    StringEntryID make(rsl::string_view characters)
    {
      if(characters.empty())
      {
        REX_WARN(LogEngine, "Trying to create a StringID with an empty string, this is not allowed.");
        return StringEntryID(); // StringID::is_none() == true
      }

      return *string_pool::store(characters);
    }
  } // namespace internal

  //-------------------------------------------------------------------------
  /**
   * Create an empty StringID.
   */
  StringID::StringID()
      : m_comparison_index(s_none_state_hash)
  {
  }

  //-------------------------------------------------------------------------
  /**
   * Create an StringID with characters.
   */
  StringID::StringID(rsl::string_view characters)
      : m_comparison_index(internal::make(characters))
  {
  }

  //-------------------------------------------------------------------------
  /**
   * Retrieve the hashed value
   */
  StringID::operator u32() const
  {
    return static_cast<u32>(m_comparison_index);
  }

  //-------------------------------------------------------------------------
  bool StringID::operator==(const StringID& other) const
  {
    return m_comparison_index == other.m_comparison_index;
  }
  //-------------------------------------------------------------------------
  bool StringID::operator!=(const StringID& other) const
  {
    return !(*this == other);
  }

  //-------------------------------------------------------------------------
  bool StringID::operator==(const StringEntryID& entryID) const
  {
    return m_comparison_index == static_cast<uint32>(entryID);
  }
  //-------------------------------------------------------------------------
  bool StringID::operator!=(const StringEntryID& entryID) const
  {
    return m_comparison_index != static_cast<uint32>(entryID);
  }

  //-------------------------------------------------------------------------
  /**
   * Converts an StringID to a readable format
   */
  rsl::string_view StringID::to_string_view() const
  {
    return string_pool::resolve(m_comparison_index);
  }

  //-------------------------------------------------------------------------
  /** True for StringID() and StringID("Invalid StringID") */
  bool StringID::is_none() const
  {
    return m_comparison_index == s_none_state_hash;
  }

  //-------------------------------------------------------------------------
  /**
   * Retrieve the hashed value
   */
  u32 StringID::value() const
  {
    return static_cast<u32>(m_comparison_index);
  }

  //-------------------------------------------------------------------------
  StringID create_sid(rsl::string_view stringView)
  {
    return StringID(stringView);
  }

  //-------------------------------------------------------------------------
  bool operator==(rsl::string_view s, const StringID& sid)
  {
    return create_sid(s) == sid;
  }
  //-------------------------------------------------------------------------
  bool operator!=(rsl::string_view s, const StringID& sid)
  {
    return create_sid(s) != sid;
  }
  //-------------------------------------------------------------------------
  bool operator==(const StringID& sid, rsl::string_view s)
  {
    return s == sid;
  }
  //-------------------------------------------------------------------------
  bool operator!=(const StringID& sid, rsl::string_view s)
  {
    return s != sid;
  }
} // namespace rex

//-------------------------------------------------------------------------
rex::StringID operator""_sid(const char* string, size_t size)
{
  return rex::StringID(rsl::string_view(string, static_cast<u32>(size))); // NOLINT(cppcoreguidelines-narrowing-conversions)
}

//-------------------------------------------------------------------------
rsl::ostream& operator<<(rsl::ostream& os, const rex::StringID& stringID)
{
  os << stringID.to_string_view();

  return os;
}