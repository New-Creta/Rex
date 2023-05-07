#include "rex_engine/string/stringentry.h"

#include <rex_std/memory.h>

namespace rex
{
  //-------------------------------------------------------------------------
  StringEntry::StringEntry(rsl::string_view characters) noexcept
      : m_characters()
  {
    m_characters = rsl::make_unique<char[]>(characters.size() + 1); 

    // Memset everything to 0 to already fill in the 0 terminator
    // and then copy the characters over
    rsl::memset(m_characters.get(), 0, characters.size() + 1);
    rsl::memcpy(m_characters.get(), characters.data(), characters.size());
  }
  //-------------------------------------------------------------------------
  StringEntry::StringEntry(StringEntry&& other) noexcept
      : m_characters(rsl::exchange(other.m_characters, {}))
  {
  }

  //-------------------------------------------------------------------------
  StringEntry::~StringEntry() = default;

  //-------------------------------------------------------------------------
  rex::StringEntry& StringEntry::operator=(StringEntry&& other) noexcept
  {
    m_characters = rsl::exchange(other.m_characters, {});

    return *this;
  }

  //-------------------------------------------------------------------------
  rsl::string_view StringEntry::characters() const
  {
    return rsl::string_view(m_characters.get(), m_characters.count());
  }

  //-------------------------------------------------------------------------
  s32 StringEntry::size() const
  {
    return m_characters.count();
  }

  //-------------------------------------------------------------------------
  StringEntryID::StringEntryID()
      : m_value(0)
  {
  }
  //-------------------------------------------------------------------------
  StringEntryID::StringEntryID(rsl::hash_result value)
      : m_value(value)
  {
  }

  //-------------------------------------------------------------------------
  bool StringEntryID::operator<(const StringEntryID& rhs) const
  {
    return m_value < rhs.m_value;
  }
  //-------------------------------------------------------------------------
  bool StringEntryID::operator>(const StringEntryID& rhs) const
  {
    return rhs.m_value < m_value;
  }

  //-------------------------------------------------------------------------
  bool StringEntryID::operator==(const StringEntryID& rhs) const
  {
    return m_value == rhs.m_value;
  }
  //-------------------------------------------------------------------------
  bool StringEntryID::operator==(const rsl::hash_result& rhs) const
  {
    return m_value == rhs;
  }
  //-------------------------------------------------------------------------
  bool StringEntryID::operator!=(const StringEntryID& rhs) const
  {
    return m_value != rhs.m_value;
  }
  //-------------------------------------------------------------------------
  bool StringEntryID::operator!=(const rsl::hash_result& rhs) const
  {
    return m_value != rhs;
  }
  //-------------------------------------------------------------------------
  StringEntryID::operator bool() const
  {
    return m_value != 0;
  }
  //-------------------------------------------------------------------------
  StringEntryID::operator rsl::hash_result() const
  {
    return m_value;
  }
} // namespace rex