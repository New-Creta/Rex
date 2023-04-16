#pragma once

#include "rex_engine/string/stringids.h"
#include "rex_engine/string/stringentry.h"
#include "rex_engine/types.h"

#include "rex_std/string.h"
#include "rex_std/ostream.h"

namespace rex
{   
	class StringID
	{
	public:
        StringID();
        StringID(const SID& name);
        StringID(const char* characters);
        StringID(const char* characters, u32 size);

        rsl::string     to_string() const;
        void            to_string(rsl::string& out) const;
        void            to_string(char** out, u32& outSize) const;

        u32             get_value() const;

        operator        u32() const;

        bool            operator==(const StringID& other) const;
        bool            operator!=(const StringID& other) const;
        
        bool            operator==(const SID& name) const;
        bool            operator!=(const SID& name) const;
        bool            operator==(const StringEntryID& entryID) const;
        bool            operator!=(const StringEntryID& entryID) const;

        bool            is_none() const;

    private:
        StringEntryID   make(const SID& name);
        StringEntryID   make(const char* characters, u32 size);

        /** Index into the StringID array (used to find String portion of the string/number pair used for comparison) */
        StringEntryID   m_comparison_index;
	};

	StringID create_sid(const SID& name);
	StringID create_sid(const char* characters);
	StringID create_sid(const char* characters, u32 size);
	StringID create_sid(const rsl::string& string);
    StringID create_sid(const rsl::string_view& stringView);

	bool operator==(const rsl::string& s, const StringID& sid);
    bool operator!=(const rsl::string& s, const StringID& sid);
    bool operator==(const StringID& sid, const rsl::string& s);
    bool operator!=(const StringID& sid, const rsl::string& s);
}

rex::StringID operator ""_sid(const char* string, size_t size);

rsl::ostream& operator<<(rsl::ostream& os, const rex::StringID& stringID);

// custom specialization of rsl::hash can be injected in namespace rsl
namespace rsl
{
    inline namespace v1
    {
        //-------------------------------------------------------------------------
        template<>
        struct hash<rex::StringID>
        {
            u32 operator()(const rex::StringID& s) const noexcept
            {
                return s.get_value();
            }
        };
    }
}