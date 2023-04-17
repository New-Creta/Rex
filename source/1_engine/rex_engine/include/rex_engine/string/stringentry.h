#pragma once

#include "rex_engine/types.h"

#include "rex_std/memory.h"
#include "rex_std/functional.h"

namespace rex
{
    class StringEntry
    {
    public:
        StringEntry(const char* chars, s32 charCount) noexcept;
        StringEntry(StringEntry&& other) noexcept;
        ~StringEntry();

        void        get_characters(const char** characters, s32& characterCount) const;
        const char* get_characters() const;

        s32         get_size() const;

    private:
        rsl::unique_ptr<char> m_characters;
        s32 m_size;
    };

    class StringEntryID
    {
    public:
        StringEntryID();
        StringEntryID(rsl::hash_result value);

        bool operator<(const StringEntryID& rhs) const;
        bool operator>(const StringEntryID& rhs) const;

        bool operator==(const StringEntryID& rhs) const;
        bool operator==(const rsl::hash_result& rhs) const;
        bool operator!=(const StringEntryID& rhs) const;
        bool operator!=(const rsl::hash_result& rhs) const;

        constexpr operator bool() const { return value != 0; }
        constexpr operator rsl::hash_result() const { return value; }

    private:
        rsl::hash_result value;
    };
}

namespace rsl
{
    inline namespace v1
    {
        template <>
        struct hash<rex::StringEntryID>
        {
            u32 operator()(const rex::StringEntryID& k) const
            {
                return static_cast<u32>(k);
            }
        };
    }
}