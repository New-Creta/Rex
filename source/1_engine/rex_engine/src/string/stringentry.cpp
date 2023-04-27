#include "rex_engine/string/stringentry.h"

#include <rex_std/memory.h>

namespace rex
{
  //-------------------------------------------------------------------------
  StringEntry::StringEntry(const char* chars, s32 charCount) noexcept
      : m_characters()
      , m_size(charCount)
  {
    m_characters.reset(static_cast<char*>(malloc(charCount + 1))); // NOLINT(cppcoreguidelines-no-malloc)

    rsl::memset(m_characters.get(), 0, charCount + 1);
    rsl::memcpy(m_characters.get(), chars, charCount);
  }
  //-------------------------------------------------------------------------
  StringEntry::StringEntry(StringEntry&& other) noexcept
      : m_characters(rsl::exchange(other.m_characters, {}))
      , m_size(rsl::exchange(other.m_size, 0))
  {
  }

  //-------------------------------------------------------------------------
  StringEntry::~StringEntry() = default;

  //-------------------------------------------------------------------------
  rex::StringEntry& StringEntry::operator=(StringEntry&& other) noexcept
  {
    m_characters = rsl::exchange(other.m_characters, {});
    m_size       = rsl::exchange(other.m_size, 0);

    return *this;
  }

  //-------------------------------------------------------------------------
  void StringEntry::get_characters(const char** characters, s32& characterCount) const
  {
    *characters    = m_characters.get();
    characterCount = m_size;
  }

  //-------------------------------------------------------------------------
  const char* StringEntry::get_characters() const
  {
    return m_characters.get();
  }

  //-------------------------------------------------------------------------
  s32 StringEntry::get_size() const
  {
    return m_size;
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