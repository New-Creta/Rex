#pragma once

#include "string/string_entry.h"
#include "string/string_ids.h"

namespace rex
{
    namespace string_pool
    {
        void resolve(const StringEntryID& entryID, char** out, size_t& outSize);
        void resolve(const StringEntryID& entryID, std::string& out);

        const StringEntry* find(const StringEntryID& entryID);

        const StringEntryID* store(const ESID& name);
        const StringEntryID* store(const char* characters, size_t size);

        uint32 hash(const std::string& s);
    };
}