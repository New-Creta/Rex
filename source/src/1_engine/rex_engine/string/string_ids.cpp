#include "rex_std_pch.h"

#include "string/string_ids.h"

namespace rex
{
    namespace conversions
    {
#define SID_NAME(name) ESID::SID_##name : return #name
        //-------------------------------------------------------------------------
        std::string to_display_string(const ESID& name)
        {
            switch (name)
            {
                case SID_NAME(None);

                    default:
                    return "None";
            }
        }
#undef SID_NAME
    }
}