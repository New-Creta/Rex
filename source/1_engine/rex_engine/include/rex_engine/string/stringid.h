#pragma once

#include "string/stringids.h"
#include "string/stringentry.h"

#include "types.h"

#include <string>
#include <vector>
#include <memory>
#include <ostream>

namespace sbt
{   
	class StringID
	{
	public:
        StringID();
        StringID(const EName& name);
        StringID(const char* characters);
        StringID(const char* characters, size_t size);

        std::string     toString() const;
        void            toString(std::string& out) const;
        void            toString(char** out, size_t& outSize) const;

        uint32          getValue() const;

        operator        uint32() const;

        bool            operator==(const StringID& other) const;
        bool            operator!=(const StringID& other) const;
        
        bool            operator==(const EName& name) const;
        bool            operator!=(const EName& name) const;
        bool            operator==(const StringEntryID& entryID) const;
        bool            operator!=(const StringEntryID& entryID) const;

        bool            isNone() const;

    private:
        StringEntryID   make(const EName& name);
        StringEntryID   make(const char* characters, size_t size);

        /** Index into the StringID array (used to find String portion of the string/number pair used for comparison) */
        StringEntryID   m_comparison_index;
	};

	//-------------------------------------------------------------------------
	inline StringID create_sid(const EName& name)
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
    inline StringID create_sid(const std::string_view& stringView)
    {
        return StringID(stringView.data(), stringView.size());
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
inline sbt::StringID operator ""_sid(const char* string, size_t size)
{
    return sbt::StringID(string, size);
}

//-------------------------------------------------------------------------
inline std::ostream& operator<<(std::ostream& os, const sbt::StringID& stringID)
{
    os << stringID.toString();

    return os;
}

// custom specialization of std::hash can be injected in namespace std
namespace std
{
	//-------------------------------------------------------------------------
    template<> struct hash<sbt::StringID>
    {
        std::size_t operator()(const sbt::StringID& s) const noexcept
        {
			return s.getValue();
        }
    };
}