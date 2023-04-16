#include "string/stringid.h"
#include "string/stringpool.h"

namespace sbt
{
    std::vector<std::unique_ptr<char>> g_allocated_strings;

#if TB_STRINGPOOL
    /**
    * Create an empty StringID.
    */
    //-------------------------------------------------------------------------
    StringID::StringID()
        :StringID(EName::SID_None)
    {}

    /**
    * Create an StringID with a hard coded string index.
    */
    //-------------------------------------------------------------------------
    StringID::StringID(const EName& name)
        :m_comparison_index(make(name))
    {}

    /**
    * Create an StringID with characters.
    */
    //-------------------------------------------------------------------------
    StringID::StringID(const char* characters)
        :m_comparison_index(make(characters, std::strlen(characters)))
    {}

    /**
    * Create an StringID with characters and a predefined size.
    */
    //-------------------------------------------------------------------------
    StringID::StringID(const char* characters, size_t size)
        :m_comparison_index(make(characters, size))
    {}

    /**
    * Converts an StringID to a readable format
    */
    //-------------------------------------------------------------------------
    std::string StringID::toString() const
    {
        std::string out;

        toString(out);
        return out;
    }

    /**
    * Converts an FName to a readable format, in place
    */
    //-------------------------------------------------------------------------
    void StringID::toString(std::string& out) const
    {
        string_pool::resolve(m_comparison_index, out);
    }

    /**
    * Convert to string buffer to avoid dynamic allocations and returns string length
    *
    * Note that a default constructed StringID returns "None" instead of ""
    */
    //-------------------------------------------------------------------------
    void StringID::toString(char** out, size_t& outSize) const
    {
        string_pool::resolve(m_comparison_index, out, outSize);
    }

    /**
    * Retrieve the hashed value
    */
    //-------------------------------------------------------------------------
    uint32 StringID::getValue() const
    {
        return m_comparison_index;
    }

    /**
    * Retrieve the hashed value
    */
    //-------------------------------------------------------------------------
    StringID::operator uint32() const
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
    bool StringID::operator==(const EName& name) const
    {
        return m_comparison_index == static_cast<uint32>(name);
    }
    //-------------------------------------------------------------------------
    bool StringID::operator!=(const EName& name) const
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
    bool StringID::isNone() const
    {
        return *this == EName::SID_None;
    }

    //-------------------------------------------------------------------------
    StringEntryID StringID::make(const EName& name)
    {
        return name == EName::SID_None
            ? StringEntryID()
            : *string_pool::store(name);
    }
    //-------------------------------------------------------------------------
    StringEntryID StringID::make(const char* characters, size_t size)
    {
        if (characters == nullptr || size == 0)
        {
            return StringEntryID();   // EName::SID_None
        }

        return *string_pool::store(characters, size);
    }
#else
    /**
    * Create an empty StringID.
    */
    //-------------------------------------------------------------------------
    StringID::StringID()
        :StringID(EName::SID_None)
    {}

    /**
    * Create an StringID with a hard coded string index.
    */
    //-------------------------------------------------------------------------
    StringID::StringID(const EName& name)
        :m_comparison_index(conversions::toDisplayString(name))
    {}

    /**
    * Create an StringID with characters.
    */
    //-------------------------------------------------------------------------
    StringID::StringID(const char* characters)
        :m_comparison_index(characters)
    {}

    /**
    * Create an StringID with characters and a predefined size.
    */
    //-------------------------------------------------------------------------
    StringID::StringID(const char* characters, size_t size)
        :m_comparison_index(characters, size)
    {}

    /**
    * Converts an StringID to a readable format
    */
    //-------------------------------------------------------------------------
    std::string StringID::toString() const
    {
        std::string out;

        toString(out);
        return out;
    }

    /**
    * Converts an FName to a readable format, in place
    */
    //-------------------------------------------------------------------------
    void StringID::toString(std::string& out) const
    {
        out = m_comparison_index;
    }

    /**
    * Convert to string buffer to avoid dynamic allocations and returns string length
    *
    * Note that a default constructed StringID returns "None" instead of ""
    */
    //-------------------------------------------------------------------------
    void StringID::toString(char** out, size_t& outSize) const
    {
        g_allocated_strings.push_back(std::unique_ptr<char>(new char[m_comparison_index.size()]));

        *out = g_allocated_strings.back().get();

        std::memcpy(*out, m_comparison_index.data(), m_comparison_index.size());

        outSize = m_comparison_index.size();
    }

    /**
    * Retrieve the hashed value
    */
    //-------------------------------------------------------------------------
    const uint32 StringID::getValue() const
    {
        return string_pool::hash(m_comparison_index);
    }

    /**
    * Retrieve the hashed value
    */
    //-------------------------------------------------------------------------
    StringID::operator const uint32() const
    {
        return string_pool::hash(m_comparison_index);
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
    bool StringID::operator==(const EName& name) const
    {
        return m_comparison_index == conversions::toDisplayString(name);
    }
    //-------------------------------------------------------------------------
    bool StringID::operator!=(const EName& name) const
    {
        return m_comparison_index != conversions::toDisplayString(name);
    }
    //-------------------------------------------------------------------------
    bool StringID::operator==(const std::string& entryID) const
    {
        return m_comparison_index == entryID;
    }
    //-------------------------------------------------------------------------
    bool StringID::operator!=(const std::string& entryID) const
    {
        return m_comparison_index != entryID;
    }

    /** True for StringID(), StringID(NAME_None) and StringID("None") */
    //-------------------------------------------------------------------------
    bool StringID::isNone() const
    {
        return m_comparison_index.empty();
    }
#endif
}