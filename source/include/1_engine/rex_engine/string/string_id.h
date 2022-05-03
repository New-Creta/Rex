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
        StringID(const StringID& other);
        StringID(StringID&& other) noexcept;
        StringID(const ESID& name);
        StringID(const char* characters);
        StringID(const char* characters, card64 size);

        StringID& operator=(const StringID& other);
        StringID& operator=(StringID&& other) noexcept;

        std::string to_string() const;

        void to_string(rtl::Out<std::string> out) const;
        void to_string(rtl::Out<char*> out, rtl::Out<card64> outSize) const;

        explicit operator const uint32() const;

        bool operator==(const StringID& other) const;
        bool operator!=(const StringID& other) const;
        bool operator==(const ESID& name) const;
        bool operator!=(const ESID& name) const;
        bool operator==(const StringEntryID& entryID) const;
        bool operator!=(const StringEntryID& entryID) const;

        bool is_none() const;

    private:
        StringEntryID make(const ESID& name);
        StringEntryID make(const char* characters, card64 size);

        /** Index into the StringID array (used to find String portion of the string/number pair used for comparison) */
        StringEntryID m_comparison_index;
    };

    StringID create_sid(const ESID& name);
    StringID create_sid(const char* characters);
    StringID create_sid(const char* characters, card64 size);
    StringID create_sid(const std::string& string);

    bool operator==(const std::string& s, const StringID& sid);
    bool operator!=(const std::string& s, const StringID& sid);
    bool operator==(const StringID& sid, const std::string& s);
    bool operator!=(const StringID& sid, const std::string& s);
}

rex::StringID operator""_sid(const char* string, card64 size);
std::ostream& operator<<(std::ostream& os, const rex::StringID& stringID);

// custom specialization of std::hash can be injected in namespace std
namespace std
{
    //-------------------------------------------------------------------------
    template <>
    struct hash<rex::StringID>
    {
        std::card64 operator()(const rex::StringID& s) const noexcept
        {
            return s.get_value();
        }
    };
}