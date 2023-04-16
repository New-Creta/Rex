#include "string/stringpool.h"
#include "string/stringentry.h"
#include "string/stringcrchash.h"

namespace sbt
{
    namespace string_pool
    {
        using EntryMap = std::unordered_map<StringEntryID, StringEntry>;

        //-------------------------------------------------------------------------
        EntryMap load_entry_map()
        {
            EntryMap map;

            for (uint32 i = 0; i < static_cast<uint32>(EName::SID_MaxHardcodedNameIndex); ++i)
            {
                std::string sid_name = conversions::toDisplayString(static_cast<EName>(i));

                StringEntryID entry_id(static_cast<uint32>(i));
                StringEntry entry(sid_name.data(), sid_name.size());

                map.insert(std::make_pair(std::move(entry_id), std::move(entry)));
            }

            return map;
        }

        //-------------------------------------------------------------------------
        EntryMap& getEntries()
        {
            static EntryMap entries = load_entry_map();
            return entries;
        }

        //-------------------------------------------------------------------------
        const StringEntryID* store(uint32 hash, const char* characters, size_t size)
        {
            auto it = getEntries().find(hash);
            if (it != std::cend(getEntries()))
            {
                assert(std::strcmp(characters, it->second.getCharacters()) == 0 && "Hash collision");

                return &it->first;
            }

            StringEntryID entry_id(hash);
            StringEntry entry(characters, size);

            auto result = getEntries().insert(std::make_pair(std::move(entry_id), std::move(entry)));
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

            assert(entry != nullptr && "Entry not found");

            entry->getCharacters(out, outSize);
        }
        //-------------------------------------------------------------------------
        void resolve(const StringEntryID& entryID, std::string& out)
        {
            const StringEntry* entry = find(entryID);

            assert(entry != nullptr && "Entry not found");

            out = std::string(entry->getCharacters(), entry->getSize());
        }

        //-------------------------------------------------------------------------
        const StringEntry* find(const StringEntryID& entryID)
        {
            auto it = getEntries().find(entryID);
            if (it == std::cend(getEntries()))
            {
                uint32 sid_name = static_cast<uint32>(EName::SID_None);

                it = getEntries().find(sid_name);

                assert(it != std::cend(getEntries()) && "SID_None not present");
            }

            return &(it->second);
        }

        //-------------------------------------------------------------------------
        const StringEntryID* store(const EName& name)
        {
            std::string sid_name = conversions::toDisplayString(name);

            return store(static_cast<uint32>(name), sid_name.data(), sid_name.size());
        }
        //-------------------------------------------------------------------------
        const StringEntryID* store(const char* characters, size_t size)
        {
            return store(crc32_hash_string(characters, size, getLookupTable()), characters, size);
        }
    }
}