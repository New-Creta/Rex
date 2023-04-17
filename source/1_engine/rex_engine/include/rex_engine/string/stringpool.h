#pragma once

#include "rex_engine/types.h"
#include "rex_engine/string/stringids.h"

#include "rex_std/string.h"

namespace rex
{   
    class StringEntry;
    class StringEntryID;

    namespace string_pool
    {
        void                    resolve(const StringEntryID& entryID, const char** out, u32& outSize);
        void                    resolve(const StringEntryID& entryID, rsl::string& out);

        const StringEntry*      find(const StringEntryID& entryID);

        const StringEntryID*    store(const SID& name);
        const StringEntryID*    store(const char* characters, u32 size);
    };
}