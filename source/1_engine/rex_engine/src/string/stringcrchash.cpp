#include "string/stringcrchash.h"

namespace sbt
{
    //-------------------------------------------------------------------------
    LookupTable& getLookupTable()
    {
        static LookupTable lookup_table = CRC::CRC_32().MakeTable();
        return lookup_table;
    }

    //-------------------------------------------------------------------------
    uint32 hash(const std::string& s)
    {
        return crc32_hash_string(s, getLookupTable());
    }
}