#include "string/stringentry.h"

namespace sbt
{
    //-------------------------------------------------------------------------
    StringEntry::StringEntry(const char* chars, size_t charCount) noexcept
        :m_characters()
        , m_size(charCount)
    {
        m_characters.reset((char*)malloc(charCount + 1));

        std::memset(m_characters.get(), 0, charCount + 1);
        std::memcpy(m_characters.get(), chars, charCount);
    }
    //-------------------------------------------------------------------------
    StringEntry::StringEntry(StringEntry&& other) noexcept
        :m_characters(std::move(other.m_characters))
        , m_size(std::move(other.m_size))
    {

    }

    //-------------------------------------------------------------------------
    StringEntry::~StringEntry() = default;

    //-------------------------------------------------------------------------
    void StringEntry::getCharacters(char** characters, size_t & characterCount) const
    {
        *characters = m_characters.get();
        characterCount = m_size;
    }

    //-------------------------------------------------------------------------
    char* StringEntry::getCharacters()
    {
        return m_characters.get();
    }
    //-------------------------------------------------------------------------
    const char* StringEntry::getCharacters() const
    {
        return m_characters.get();
    }

    //-------------------------------------------------------------------------
    size_t StringEntry::getSize() const
    {
        return m_size;
    }

    //-------------------------------------------------------------------------
    StringEntryID::StringEntryID()
        :value(0)
    {}
    //-------------------------------------------------------------------------
    StringEntryID::StringEntryID(uint32 value)
        : value(value)
    {}

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