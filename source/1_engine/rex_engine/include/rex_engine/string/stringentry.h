#pragma once

#include "types.h"

#include <cstring>
#include <memory>
#include <functional>

namespace sbt
{
    class StringEntry
    {
    public:
        StringEntry(const char* chars, size_t charCount) noexcept;
        StringEntry(StringEntry&& other) noexcept;
        ~StringEntry();

        void getCharacters(char** characters, size_t& characterCount) const;

        char* getCharacters();
        const char* getCharacters() const;

        size_t getSize() const;

    private:
        std::unique_ptr<char> m_characters;
        size_t m_size;
    };

    class StringEntryID
    {
    public:
        StringEntryID();
        StringEntryID(uint32 value);

        bool operator<(const StringEntryID& rhs) const;
        bool operator>(const StringEntryID& rhs) const;

        bool operator==(const StringEntryID& rhs) const;
        bool operator==(const uint32& rhs) const;
        bool operator!=(const StringEntryID& rhs) const;
        bool operator!=(const uint32& rhs) const;

        constexpr operator bool() const { return value != 0; }
        constexpr operator uint32() const { return value; }

    private:
        uint32 value;
    };
}

namespace std
{
    template <>
    struct hash<sbt::StringEntryID>
    {
        std::size_t operator()(const sbt::StringEntryID& k) const
        {
            return static_cast<uint32>(k);
        }
    };
}