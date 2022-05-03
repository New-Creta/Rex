#include "rex_engine_pch.h"

#include "string/string_id.h"
#include "string/string_pool.h"

namespace rex
{
    rtl::Vector<rtl::UniquePtr<char>> g_allocated_strings;

    /**
    * Create an empty StringID.
    */
    //-------------------------------------------------------------------------
    StringID::StringID()
        : StringID(ESID::SID_None)
    {
    }

    /**
    * Copy ctor StringID
    */
    //-------------------------------------------------------------------------
    StringID(const StringID& other);
        :m_comparison_index(other.m_comparison_index)
    {

    }

    /**
    * Move ctor StringID
    */
    //-------------------------------------------------------------------------
    StringID(StringID&& other) noexcept
        :m_comparison_index(std::exchange(other.m_comparison_index, make(ESID::None)))
    {

    }

    /**
    * Create an StringID with a hard coded string index.
    */
    //-------------------------------------------------------------------------
    StringID::StringID(const ESID& name)
        : m_comparison_index(make(name))
    {
    }

    /**
    * Create an StringID with characters.
    */
    //-------------------------------------------------------------------------
    StringID::StringID(const char* characters)
        : m_comparison_index(make(characters, std::strlen(characters)))
    {
    }

    /**
    * Create an StringID with characters and a predefined size.
    */
    //-------------------------------------------------------------------------
    StringID::StringID(const char* characters, card64 size)
        : m_comparison_index(make(characters, size))
    {
    }

    //-------------------------------------------------------------------------
    StringID& StringID::operator=(const StringID& other)
    {
        m_comparison_index = other.m_comparison_index;
    }
    //-------------------------------------------------------------------------
    StringID& StringID::operator=(StringID&& other) noexcept
    {
        m_comparison_index = std::exchange(other.m_comparison_index, make(ESID::SID_None));
    }

    /**
    * Converts an StringID to a readable format
    */
    //-------------------------------------------------------------------------
    std::string StringID::to_string() const
    {
        std::string out;

        to_string(out);
        return out;
    }

    /**
    * Converts an FName to a readable format, in place
    */
    //-------------------------------------------------------------------------
    void StringID::to_string(rtl::Out<std::string> out) const
    {
        string_pool::resolve(m_comparison_index, out);
    }

    /**
    * Convert to string buffer to avoid dynamic allocations and returns string length
    *
    * Note that a default constructed StringID returns "None" instead of ""
    */
    //-------------------------------------------------------------------------
    void StringID::to_string(rtl::Out<char*> out, rtl::Out<card64> outSize) const
    {
        string_pool::resolve(m_comparison_index, out, outSize);
    }

    /**
    * Retrieve the hashed value
    */
    //-------------------------------------------------------------------------
    StringID::operator const uint32() const
    {
        return m_comparison_index;
    }

    /**
    * Check to see if this StringID matches the other StringID.
    */
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
    bool StringID::operator==(const ESID& name) const
    {
        return m_comparison_index == static_cast<uint32>(name);
    }
    //-------------------------------------------------------------------------
    bool StringID::operator!=(const ESID& name) const
    {
        return m_comparison_index != static_cast<uint32>(name);
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

    /** True for StringID(), StringID(NAME_None) and StringID("None") */
    //-------------------------------------------------------------------------
    bool StringID::is_none() const
    {
        return *this == ESID::SID_None;
    }

    //-------------------------------------------------------------------------
    StringEntryID StringID::make(const ESID& name)
    {
        return name == ESID::SID_None ? StringEntryID() : *string_pool::store(name);
    }
    //-------------------------------------------------------------------------
    StringEntryID StringID::make(const char* characters, card64 size)
    {
        if (characters == nullptr || size == 0)
        {
            return StringEntryID();  // ESID::SID_None
        }

        return *string_pool::store(characters, size);
    }

        //-------------------------------------------------------------------------
    StringID create_sid(const ESID& name)
    {
        return StringID(name);
    }
    //-------------------------------------------------------------------------
    StringID create_sid(const char* characters)
    {
        return StringID(characters, std::strlen(characters));
    }
    //-------------------------------------------------------------------------
    StringID create_sid(const char* characters, card64 size)
    {
        return StringID(characters, size);
    }
    //-------------------------------------------------------------------------
    StringID create_sid(const std::string& string)
    {
        return StringID(string.data(), string.size());
    }

    //-------------------------------------------------------------------------
    bool operator==(const std::string& s, const StringID& sid)
    {
        return create_sid(s) == sid;
    }
    //-------------------------------------------------------------------------
    bool operator!=(const std::string& s, const StringID& sid)
    {
        return create_sid(s) != sid;
    }
    //-------------------------------------------------------------------------
    bool operator==(const StringID& sid, const std::string& s)
    {
        return s == sid;
    }
    //-------------------------------------------------------------------------
    bool operator!=(const StringID& sid, const std::string& s)
    {
        return s != sid;
    }
}

//-------------------------------------------------------------------------
rex::StringID operator""_sid(const char* string, card64 size)
{
    return rex::StringID(string, size);
}

//-------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const rex::StringID& stringID)
{
    os << stringID.to_string();

    return os;
}