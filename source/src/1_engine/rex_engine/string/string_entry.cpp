#include "rex_std_pch.h"

#include "string/string_entry.h"

namespace rex
{
    //-------------------------------------------------------------------------
    StringEntry::StringEntry(const char* chars, size_t charCount) noexcept : m_characters(), m_size(charCount)
    {
        m_characters.reset((char*)malloc(charCount + 1));

        std::memset(m_characters.get(), 0, charCount + 1);
        std::memcpy(m_characters.get(), chars, charCount);
    }
    //-------------------------------------------------------------------------
    StringEntry::StringEntry(StringEntry&& other) noexcept : m_characters(std::move(other.m_characters)), m_size(std::move(other.m_size))
    {
    }

    //-------------------------------------------------------------------------
    StringEntry::~StringEntry() = default;

    //-------------------------------------------------------------------------
    void StringEntry::get_characters(char** characters, size_t& characterCount) const
    {
        *characters = m_characters.get();
        characterCount = m_size;
    }

    //-------------------------------------------------------------------------
    char* StringEntry::get_characters()
    {
        return m_characters.get();
    }
    //-------------------------------------------------------------------------
    const char* StringEntry::get_characters() const
    {
        return m_characters.get();
    }

    //-------------------------------------------------------------------------
    size_t StringEntry::get_size()
    {
        return m_size;
    }

    //-------------------------------------------------------------------------
    const size_t StringEntry::get_size() const
    {
        return m_size;
    }

    //-------------------------------------------------------------------------
    StringEntryID::StringEntryID()
        : value(0)
    {
    }
    //-------------------------------------------------------------------------
    StringEntryID::StringEntryID(uint32 value)
        : value(value)
    {
    }

    //-------------------------------------------------------------------------
    bool StringEntryID::operator<(const StringEntryID& rhs) const
    {
        return value < rhs.value;
    }
    //-------------------------------------------------------------------------
    bool StringEntryID::operator>(const StringEntryID& rhs) const
    {
        return rhs.value < value;
    }

    //-------------------------------------------------------------------------
    bool StringEntryID::operator==(const StringEntryID& rhs) const
    {
        return value == rhs.value;
    }
    //-------------------------------------------------------------------------
    bool StringEntryID::operator==(const uint32& rhs) const
    {
        return value == rhs;
    }
    //-------------------------------------------------------------------------
    bool StringEntryID::operator!=(const StringEntryID& rhs) const
    {
        return value != rhs.value;
    }
    //-------------------------------------------------------------------------
    bool StringEntryID::operator!=(const uint32& rhs) const
    {
        return value != rhs;
    }
}