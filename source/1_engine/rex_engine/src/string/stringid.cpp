#include "rex_engine/string/stringid.h"
#include "rex_engine/string/stringpool.h"

#include "rex_std/vector.h"
#include "rex_std/memory.h"

namespace rex
{
    /**
    * Create an empty StringID.
    */
    //-------------------------------------------------------------------------
    StringID::StringID()
        :StringID(SID::None)
    {}

    /**
    * Create an StringID with a hard coded string index.
    */
    //-------------------------------------------------------------------------
    StringID::StringID(const SID& name)
        :m_comparison_index(make(name))
    {}

    /**
    * Create an StringID with characters.
    */
    //-------------------------------------------------------------------------
    StringID::StringID(const char* characters)
        :m_comparison_index(make(characters, rsl::strlen(characters)))
    {}

    /**
    * Create an StringID with characters and a predefined size.
    */
    //-------------------------------------------------------------------------
    StringID::StringID(const char* characters, u32 size)
        :m_comparison_index(make(characters, size))
    {}

    /**
    * Converts an StringID to a readable format
    */
    //-------------------------------------------------------------------------
    rsl::string StringID::to_string() const
    {
        rsl::string out;

        to_string(out);
        return out;
    }

    /**
    * Converts an FName to a readable format, in place
    */
    //-------------------------------------------------------------------------
    void StringID::to_string(rsl::string& out) const
    {
        string_pool::resolve(m_comparison_index, out);
    }

    /**
    * Convert to string buffer to avoid dynamic allocations and returns string length
    *
    * Note that a default constructed StringID returns "None" instead of ""
    */
    //-------------------------------------------------------------------------
    void StringID::to_string(char** out, u32& outSize) const
    {
        string_pool::resolve(m_comparison_index, out, outSize);
    }

    /**
    * Retrieve the hashed value
    */
    //-------------------------------------------------------------------------
    uint32 StringID::get_value() const
    {
        return m_comparison_index;
    }

    /**
    * Retrieve the hashed value
    */
    //-------------------------------------------------------------------------
    StringID::operator u32() const
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
    bool StringID::operator==(const SID& name) const
    {
        return m_comparison_index == static_cast<uint32>(name);
    }
    //-------------------------------------------------------------------------
    bool StringID::operator!=(const SID& name) const
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

    //-------------------------------------------------------------------------
    /** True for StringID(), StringID(NAME_None) and StringID("None") */
    bool StringID::is_none() const
    {
        return *this == SID::None;
    }

    //-------------------------------------------------------------------------
    StringEntryID StringID::make(const SID& name)
    {
        return name == SID::None
            ? StringEntryID()
            : *string_pool::store(name);
    }
    //-------------------------------------------------------------------------
    StringEntryID StringID::make(const char* characters, u32 size)
    {
        if (characters == nullptr || size == 0)
        {
            return StringEntryID();   // SID::None
        }

        return *string_pool::store(characters, size);
    }

    //-------------------------------------------------------------------------
	StringID create_sid(const SID& name)
	{
		return StringID(name);
	}
	//-------------------------------------------------------------------------
	StringID create_sid(const char* characters)
	{
		return StringID(characters, rsl::strlen(characters));
	}
	//-------------------------------------------------------------------------
	StringID create_sid(const char* characters, u32 size)
	{
		return StringID(characters, size);
	}
	//-------------------------------------------------------------------------
	StringID create_sid(const rsl::string& string)
	{
		return StringID(string.data(), string.size());
	}
    //-------------------------------------------------------------------------
    StringID create_sid(const rsl::string_view& stringView)
    {
        return StringID(stringView.data(), stringView.size());
    }

	//-------------------------------------------------------------------------
	bool operator==(const rsl::string& s, const StringID& sid)
	{
		return create_sid(s) == sid;
	}
    //-------------------------------------------------------------------------
    bool operator!=(const rsl::string& s, const StringID& sid)
    {
        return create_sid(s) != sid;
    }
    //-------------------------------------------------------------------------
    bool operator==(const StringID& sid, const rsl::string& s)
    {
        return s == sid;
    }
    //-------------------------------------------------------------------------
    bool operator!=(const StringID& sid, const rsl::string& s)
    {
        return s != sid;
    }
}

//-------------------------------------------------------------------------
rex::StringID operator ""_sid(const char* string, size_t size)
{
    return rex::StringID(string, static_cast<u32>(size));
}

//-------------------------------------------------------------------------
rsl::ostream& operator<<(rsl::ostream& os, const rex::StringID& stringID)
{
    os << stringID.to_string();

    return os;
}