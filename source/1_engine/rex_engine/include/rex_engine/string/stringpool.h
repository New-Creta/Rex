#pragma once

#include "string/stringids.h"

#include <string>
#include <unordered_map>

#define TB_STRINGPOOL 1

namespace sbt
{   
    class StringEntry;
    class StringEntryID;

    namespace string_pool
    {
        void                    resolve(const StringEntryID& entryID, char** out, size_t& outSize);
        void                    resolve(const StringEntryID& entryID, std::string& out);

        const StringEntry*      find(const StringEntryID& entryID);

        const StringEntryID*    store(const EName& name);
        const StringEntryID*    store(const char* characters, size_t size);
    };
}