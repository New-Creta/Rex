#pragma once

namespace rex
{
    class StringEntry
    {
    public:
        StringEntry(const char* chars, size_t charCount) noexcept;
        StringEntry(StringEntry&& other) noexcept;
        ~StringEntry();

        void get_characters(char** characters, size_t& characterCount) const;

        char* get_characters();
        const char* get_characters() const;

        size_t get_size();
        const size_t get_size() const;

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

        constexpr operator bool() const
        {
            return value != 0;
        }
        constexpr operator uint32() const
        {
            return value;
        }

    private:
        uint32 value;
    };
}

namespace std
{
    template <>
    struct hash<rex::StringEntryID>
    {
        std::size_t operator()(const rex::StringEntryID& k) const
        {
            return static_cast<uint32>(k);
        }
    };
}