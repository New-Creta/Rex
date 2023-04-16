#include "rex_engine/string/stringentry.h"

namespace rex
{
    //-------------------------------------------------------------------------
    StringEntry::StringEntry(const char* chars, s32 charCount) noexcept
        :m_characters()
        ,m_size(charCount)
    {
        m_characters.reset((char*)malloc(charCount + 1));

        rsl::memset(m_characters.get(), 0, charCount + 1);
        rsl::memcpy(m_characters.get(), chars, charCount);
    }
    //-------------------------------------------------------------------------
    StringEntry::StringEntry(StringEntry&& other) noexcept
        :m_characters(rsl::move(other.m_characters))
        , m_size(rsl::move(other.m_size))
    {

    }

    //-------------------------------------------------------------------------
    StringEntry::~StringEntry() = default;

    //-------------------------------------------------------------------------
    void StringEntry::get_characters(const char** characters, s32& characterCount) const
    {
        *characters = m_characters.get();
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
        :value(0)
    {}
    //-------------------------------------------------------------------------
    StringEntryID::StringEntryID(rsl::hash_result value)
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
    bool StringEntryID::operator==(const rsl::hash_result& rhs) const
    {
        return value == rhs;
    }
    //-------------------------------------------------------------------------
    bool StringEntryID::operator!=(const StringEntryID& rhs) const
    {
        return value != rhs.value;
    }
    //-------------------------------------------------------------------------
    bool StringEntryID::operator!=(const rsl::hash_result& rhs) const
    {
        return value != rhs;
    }
}