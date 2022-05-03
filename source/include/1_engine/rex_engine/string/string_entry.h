#pragma once

namespace rex
{
    class StringEntry
    {
    public:
        StringEntry();
        StringEntry(const StringEntry& other) = delete;
        StringEntry(StringEntry&& other) noexcept;
        StringEntry(const char* chars, card64 charCount) noexcept;
        ~StringEntry();

        StringEntry& operator=(const StringEntry& other) = delete;
        StringEntry& operator=(StringEntry&& other) noexcept;

        void characters(char** characters, card64& characterCount) const;

        const char* characters() const;
        const card64 size() const;

    private:
        rtl::UniquePtr<char8> m_characters;
        card64 m_size;
    };

    class StringEntryID
    {
    public:
        StringEntryID();
        StringEntryID(const StringEntryID& other);
        StringEntryID(StringEntryID&& other) noexcept;
        StringEntryID(uint32 value);

        StringEntryID& operator=(const StringEntryID& other);
        StringEntryID& operator=(StringEntryID&& other) noexcept;

        bool operator<(const StringEntryID& rhs) const;
        bool operator>(const StringEntryID& rhs) const;

        bool operator==(const StringEntryID& rhs) const;
        bool operator==(uint32 rhs) const;
        bool operator!=(const StringEntryID& rhs) const;
        bool operator!=(uint32 rhs) const;

        explicit operator bool() const;
        explicit operator uint32() const;

    private:
        uint32 cvalue;
    };
}

namespace std
{
    template <>
    struct hash<rex::StringEntryID>
    {
        std::card64 operator()(const rex::StringEntryID& k) const
        {
            return static_cast<uint32>(k);
        }
    };
}