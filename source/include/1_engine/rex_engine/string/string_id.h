#pragma once

#include "string/string_ids.h"
#include "string/string_entry.h"

#include <ostream>

namespace rex
{
    class StringID
    {
    public:
        StringID();
        StringID(const ESID& name);
        StringID(const char* characters);
        StringID(const char* characters, size_t size);

        std::string to_string() const;
        void to_string(std::string& out) const;
        void to_string(char** out, size_t& outSize) const;

        const uint32 get_value() const;

        operator const uint32() const;

        bool operator==(const StringID& other) const;
        bool operator!=(const StringID& other) const;
        bool operator==(const ESID& name) const;
        bool operator!=(const ESID& name) const;
        bool operator==(const StringEntryID& entryID) const;
        bool operator!=(const StringEntryID& entryID) const;

        bool is_none() const;

    private:
        StringEntryID make(const ESID& name);
        StringEntryID make(const char* characters, size_t size);

        /** Index into the StringID array (used to find String portion of the string/number pair used for comparison) */
        StringEntryID m_comparison_index;
    };

    //-------------------------------------------------------------------------
    inline StringID create_sid(const ESID& name)
    {
        return StringID(name);
    }
    //-------------------------------------------------------------------------
    inline StringID create_sid(const char* characters)
    {
        return StringID(characters, std::strlen(characters));
    }
    //-------------------------------------------------------------------------
    inline StringID create_sid(const char* characters, size_t size)
    {
        return StringID(characters, size);
    }
    //-------------------------------------------------------------------------
    inline StringID create_sid(const std::string& string)
    {
        return StringID(string.data(), string.size());
    }

    //-------------------------------------------------------------------------
    inline bool operator==(const std::string& s, const StringID& sid)
    {
        return create_sid(s) == sid;
    }
    //-------------------------------------------------------------------------
    inline bool operator!=(const std::string& s, const StringID& sid)
    {
        return create_sid(s) != sid;
    }
    //-------------------------------------------------------------------------
    inline bool operator==(const StringID& sid, const std::string& s)
    {
        return s == sid;
    }
    //-------------------------------------------------------------------------
    inline bool operator!=(const StringID& sid, const std::string& s)
    {
        return s != sid;
    }
}

//-------------------------------------------------------------------------
inline rex::StringID operator""_sid(const char* string, size_t size)
{
    return rex::StringID(string, size);
}

//-------------------------------------------------------------------------
inline std::ostream& operator<<(std::ostream& os, const rex::StringID& stringID)
{
    os << stringID.to_string();

    return os;
}

// custom specialization of std::hash can be injected in namespace std
namespace std
{
    //-------------------------------------------------------------------------
    template <>
    struct hash<rex::StringID>
    {
        std::size_t operator()(const rex::StringID& s) const noexcept
        {
            return s.get_value();
        }
    };
}