#include "rex_std_pch.h"

#include "string/string_pool.h"
#include "string/string_entry.h"

#pragma warning(push)
#pragma warning(disable : 4127)

#include <CRC.h>

#pragma warning(pop)

namespace rex
{
    namespace string_pool
    {
        using EntryMap = std::unordered_map<StringEntryID, StringEntry>;
        using LookupTable = CRC::Table<uint32_t, (uint16_t)32u>;

        //-------------------------------------------------------------------------
        EntryMap load_entry_map()
        {
            EntryMap map;

            for (uint32 i = 0; i < static_cast<uint32>(ESID::SID_MaxHardcodedNameIndex); ++i)
            {
                std::string sid_name = conversions::to_display_string(static_cast<ESID>(i));

                StringEntryID entry_id(static_cast<uint32>(i));
                StringEntry entry(sid_name.data(), sid_name.size());

                map.insert(std::make_pair(std::move(entry_id), std::move(entry)));
            }

            return map;
        }

        //-------------------------------------------------------------------------
        LookupTable& get_lookup_table()
        {
            static LookupTable lookup_table = CRC::CRC_32().MakeTable();
            return lookup_table;
        }
        //-------------------------------------------------------------------------
        EntryMap& get_entries()
        {
            static EntryMap entries = load_entry_map();
            return entries;
        }

        //-------------------------------------------------------------------------
        template <typename TLookupTable>
        uint32 crc32_hash_string(const char* characters, size_t size, const TLookupTable& lookupTable)
        {
            return CRC::Calculate(characters, size, lookupTable);
        }
        //-------------------------------------------------------------------------
        template <typename TLookupTable>
        uint32 crc32_hash_string(const std::string& s, const TLookupTable& lookupTable)
        {
            return crc32_hash_string(s.data(), s.size(), lookupTable);
        }

        //-------------------------------------------------------------------------
        const StringEntryID* store(uint32 hash, const char* characters, size_t size)
        {
            auto it = get_entries().find(hash);
            if (it != std::cend(get_entries()))
            {
                R_ASSERT_X(std::strcmp(characters, it->second.get_characters()) == 0, "Hash collision");

                return &it->first;
            }

            StringEntryID entry_id(hash);
            StringEntry entry(characters, size);

            auto result = get_entries().insert(std::make_pair(std::move(entry_id), std::move(entry)));
            if (result.second)
            {
                it = result.first;

                return &it->first;
            }

            return nullptr;
        }

        //-------------------------------------------------------------------------
        void resolve(const StringEntryID& entryID, char** out, size_t& outSize)
        {
            const StringEntry* entry = find(entryID);

            R_ASSERT_X(entry != nullptr, "Entry not found");

            entry->get_characters(out, outSize);
        }
        //-------------------------------------------------------------------------
        void resolve(const StringEntryID& entryID, std::string& out)
        {
            const StringEntry* entry = find(entryID);

            R_ASSERT_X(entry != nullptr, "Entry not found");

            out = std::string(entry->get_characters(), entry->get_size());
        }

        //-------------------------------------------------------------------------
        const StringEntry* find(const StringEntryID& entryID)
        {
            auto it = get_entries().find(entryID);
            if (it == std::cend(get_entries()))
            {
                uint32 sid_name = static_cast<uint32>(ESID::SID_None);

                it = get_entries().find(sid_name);

                R_ASSERT_X(it != std::cend(get_entries()), "SID_None not present");
            }

            return &(it->second);
        }

        //-------------------------------------------------------------------------
        const StringEntryID* store(const ESID& name)
        {
            std::string sid_name = conversions::to_display_string(name);

            return store(static_cast<uint32>(name), sid_name.data(), sid_name.size());
        }
        //-------------------------------------------------------------------------
        const StringEntryID* store(const char* characters, size_t size)
        {
            return store(crc32_hash_string(characters, size, get_lookup_table()), characters, size);
        }

        //-------------------------------------------------------------------------
        uint32 hash(const std::string& s)
        {
            return crc32_hash_string(s, get_lookup_table());
        }
    }
}