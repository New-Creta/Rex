#pragma once

#if defined _MSC_VER
#   pragma warning (push)
#   pragma warning (disable : 4127)
#endif

#include "types.h"

#include "CRC.h"

#if defined _MSC_VER
#   pragma warning (pop)
#endif

namespace sbt
{
    using LookupTable = CRC::Table<uint32_t, (uint16_t)32u>;

    //-------------------------------------------------------------------------
    LookupTable& getLookupTable();

    //-------------------------------------------------------------------------
    uint32 hash(const std::string& s);

    //-------------------------------------------------------------------------
    template<typename TLookupTable>
    uint32 crc32_hash_string(const char* characters, size_t size, const TLookupTable& lookupTable)
    {
        return CRC::Calculate(characters, size, lookupTable);
    }

    //-------------------------------------------------------------------------
    template<typename TLookupTable>
    uint32 crc32_hash_string(const std::string& s, const TLookupTable& lookupTable)
    {
        return crc32_hash_string(s.data(), s.size(), lookupTable);
    }
}