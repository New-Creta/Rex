#include "rex_engine/string/stringentry.h"

#include <rex_std/memory.h>

namespace rex
{
  //-------------------------------------------------------------------------
  StringEntry::StringEntry(rsl::string_view characters) noexcept
      : m_characters()
  {
    m_characters = rsl::make_unique<char[]>(characters.size() + 1); // NOLINT(modernize-avoid-c-arrays)

    rsl::memcpy(m_characters.get(), characters.data(), characters.size());
    m_characters[m_characters.count()] = 0;
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
      : m_value(s_none_state_hash)
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
    return m_value != s_none_state_hash;
  }
  //-------------------------------------------------------------------------
  StringEntryID::operator rsl::hash_result() const
  {
    return m_value;
  }
} // namespace rex