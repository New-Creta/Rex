#include "rex_engine/string/stringviewid.h"

#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/string/stringid.h"
#include "rex_engine/log.h"
#include "rex_engine/string/stringpool.h"
#include "rex_std/bonus/types.h"

namespace rex
{
    //-------------------------------------------------------------------------
    /**
     * Create an empty StringViewID.
     */
    StringViewID::StringViewID()
        : m_comparison_hash(StringEntryID::s_none_state_hash)
    {
    }

    //-------------------------------------------------------------------------
    /**
     * Create an StringViewID with characters.
     */
    StringViewID::StringViewID(rsl::string_view stringView)
        : m_comparison_hash(string_pool::make(stringView))
        , m_string_view(stringView)
    {
    }

    //-------------------------------------------------------------------------
    /**
     * Retrieve the hashed value
     */
    StringViewID::operator u32() const
    {
        return static_cast<u32>(m_comparison_hash);
    }

    //-------------------------------------------------------------------------
    bool StringViewID::operator==(const StringViewID& other) const
    {
        return m_comparison_hash == other.m_comparison_hash;
    }
    //-------------------------------------------------------------------------
    bool StringViewID::operator!=(const StringViewID& other) const
    {
        return !(*this == other);
    }

    //-------------------------------------------------------------------------
    bool StringViewID::operator==(const StringEntryID& entryID) const
    {
        return m_comparison_hash == static_cast<uint32>(entryID);
    }
    //-------------------------------------------------------------------------
    bool StringViewID::operator!=(const StringEntryID& entryID) const
    {
        return m_comparison_hash != static_cast<uint32>(entryID);
    }

    //-------------------------------------------------------------------------
    /**
     * Converts an StringViewID to a readable format
     */
    rsl::string_view StringViewID::to_string_view() const
    {
        return m_string_view;
    }

    //-------------------------------------------------------------------------
    /** True for StringViewID() and StringViewID("Invalid StringViewID") */
    bool StringViewID::is_none() const
    {
        return m_comparison_hash == StringEntryID::s_none_state_hash;
    }

    //-------------------------------------------------------------------------
    /**
     * Retrieve the hashed value
     */
    u32 StringViewID::value() const
    {
        return static_cast<u32>(m_comparison_hash);
    }

    //-------------------------------------------------------------------------
    bool operator==(rsl::string_view s, const StringViewID& sid)
    {
        return StringViewID(s) == sid;
    }
    //-------------------------------------------------------------------------
    bool operator!=(rsl::string_view s, const StringViewID& sid)
    {
        return StringViewID(s) != sid;
    }
    //-------------------------------------------------------------------------
    bool operator==(const StringViewID& sid, rsl::string_view s)
    {
        return s == sid;
    }

    //-------------------------------------------------------------------------
    bool operator!=(const StringViewID& sid, rsl::string_view s)
    {
        return s != sid;
    }

    //-------------------------------------------------------------------------
    bool operator==(const StringViewID& svid, const StringID& sid)
    {
        return svid.value() == sid.value();
    }

    //-------------------------------------------------------------------------
    bool operator!=(const StringViewID& svid, const StringID& sid)
    {
        return svid.value() == sid.value();
    }

} // namespace rex

//-------------------------------------------------------------------------
rex::StringViewID operator""_sid(const char* string, size_t size)
{
    return rex::StringViewID(rsl::string_view(string, static_cast<u32>(size))); // NOLINT(cppcoreguidelines-narrowing-conversions)
}

//-------------------------------------------------------------------------
rsl::ostream& operator<<(rsl::ostream& os, const rex::StringViewID& stringID)
{
    os << stringID.to_string_view();

    return os;
}