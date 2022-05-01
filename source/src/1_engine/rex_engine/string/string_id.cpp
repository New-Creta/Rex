#include "rex_std_pch.h"

#include "string/string_id.h"
#include "string/string_pool.h"

namespace rex
{
    std::vector<std::unique_ptr<char>> g_allocated_strings;

    /**
    * Create an empty StringID.
    */
    //-------------------------------------------------------------------------
    StringID::StringID()
        : StringID(ESID::SID_None)
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
    StringID::StringID(const char* characters, size_t size)
        : m_comparison_index(make(characters, size))
    {
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
    void StringID::to_string(std::string& out) const
    {
        string_pool::resolve(m_comparison_index, out);
    }

    /**
    * Convert to string buffer to avoid dynamic allocations and returns string length
    *
    * Note that a default constructed StringID returns "None" instead of ""
    */
    //-------------------------------------------------------------------------
    void StringID::to_string(char** out, size_t& outSize) const
    {
        string_pool::resolve(m_comparison_index, out, outSize);
    }

    /**
    * Retrieve the hashed value
    */
    //-------------------------------------------------------------------------
    const uint32 StringID::get_value() const
    {
        return m_comparison_index;
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
    StringEntryID StringID::make(const char* characters, size_t size)
    {
        if (characters == nullptr || size == 0)
        {
            return StringEntryID();  // ESID::SID_None
        }

        return *string_pool::store(characters, size);
    }
}