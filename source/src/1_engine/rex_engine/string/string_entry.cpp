#include "rex_std_pch.h"

#include "string/string_entry.h"

namespace rex
{
    //-------------------------------------------------------------------------
    StringEntry::StringEntry()
        :m_characters(nullptr)
        ,m_size(0)
    {}
    //-------------------------------------------------------------------------
    StringEntry::StringEntry(StringEntry&& other) noexcept 
        : m_characters(std::move(other.m_characters))
        , m_size(std::move(other.m_size))
    {
    }
    //-------------------------------------------------------------------------
    StringEntry::StringEntry(const char* chars, size_t charCount) noexcept 
        : m_characters()
        , m_size(charCount)
    {
        m_characters.reset((char*)malloc(charCount + 1));

        std::memset(m_characters.get(), 0, charCount + 1);
        std::memcpy(m_characters.get(), chars, charCount);
    }

    //-------------------------------------------------------------------------
    StringEntry::~StringEntry() = default;

    //-------------------------------------------------------------------------
    void StringEntry::characters(char** characters, size_t& characterCount) const
    {
        *characters = m_characters.get();
        characterCount = m_size;
    }

    //-------------------------------------------------------------------------
    const char* StringEntry::characters() const
    {
        return m_characters.get();
    }
    //-------------------------------------------------------------------------
    const size_t StringEntry::size() const
    {
        return m_size;
    }

    //-------------------------------------------------------------------------
    StringEntryID::StringEntryID()
        : StringEntryID(0)
    {
    }
    //-------------------------------------------------------------------------
    StringEntryID::StringEntryID(const StringEntryID& other)
        : m_value(other.value)
    {}
    //-------------------------------------------------------------------------
    StringEntryID::StringEntryID(StringEntryID&& other)
        : m_value(std::exchange(other.value, 0))
    {}
    //-------------------------------------------------------------------------
    StringEntryID::StringEntryID(uint32 value)
        : m_value(value)
    {}

    //-------------------------------------------------------------------------
    StringEntryID& operator=(const StringEntryID& other)
    {
        m_value = other.m_value;

        return *this;
    }
    //-------------------------------------------------------------------------
    StringEntryID& operator=(StringEntryID&& other) noexcept
    {
        m_value = std::exchange(other.m_value, 0);

        return *this;
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
    bool StringEntryID::operator==(uint32 rhs) const
    {
        return m_value == rhs;
    }
    //-------------------------------------------------------------------------
    bool StringEntryID::operator!=(const StringEntryID& rhs) const
    {
        return m_value != rhs.m_value;
    }
    //-------------------------------------------------------------------------
    bool StringEntryID::operator!=(uint32 rhs) const
    {
        return m_value != rhs;
    }

    //-------------------------------------------------------------------------
    operator bool() const
    {
        return m_value != 0;
    }
    //-------------------------------------------------------------------------
    operator uint32() const
    {
        return m_value;
    }
}