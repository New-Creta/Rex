#pragma once

#include "rex_engine/string/stringentry.h"
#include "rex_engine/types.h"
#include "rex_std/bonus/functional.h"
#include "rex_std/memory.h"
#include "rex_std/ostream.h"
#include "rex_std/string.h"

#include <cstddef>

namespace rex
{
    class StringID;

    class StringViewID
    {
    public:
        StringViewID();
        explicit StringViewID(rsl::string_view stringView);

        explicit operator u32() const;

        bool operator==(const StringViewID& other) const;
        bool operator!=(const StringViewID& other) const;

        bool operator==(const StringEntryID& entryID) const;
        bool operator!=(const StringEntryID& entryID) const;

        rsl::string_view to_string_view() const;
        bool is_none() const;
        u32 value() const;

    private:
        StringEntryID m_comparison_hash;
        rsl::string_view m_string_view;
    };

    bool operator==(rsl::string_view s, const StringViewID& sid);
    bool operator!=(rsl::string_view s, const StringViewID& sid);
    bool operator==(const StringViewID& sid, rsl::string_view s);
    bool operator!=(const StringViewID& sid, rsl::string_view s);
    bool operator==(const StringViewID& svid, const StringID& sid);
    bool operator!=(const StringViewID& svid, const StringID& sid);
}

rex::StringViewID operator""_sid(const char* string, size_t size);

rsl::ostream& operator<<(rsl::ostream& os, const rex::StringViewID& stringID);

// custom specialization of rsl::hash can be injected in namespace rsl
namespace rsl
{
    inline namespace v1
    {
        //-------------------------------------------------------------------------
        template <>
        struct hash<rex::StringViewID>
        {
            rsl::hash_result operator()(const rex::StringViewID& s) const noexcept
            {
                return s.value();
            }
        };


        //-------------------------------------------------------------------------
        template <>
        struct rsl::formatter<rex::StringViewID>
        {
            constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

            template <typename FormatContext>
            auto format(const rex::StringViewID& sid, FormatContext& ctx)
            {
                // Format your type's output here
                return rsl::format_to(ctx.out(), "{}", sid.to_string_view());
            }
        };
    } // namespace v1
} // namespace rsl