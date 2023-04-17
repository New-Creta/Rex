#pragma once

#include "rex_std/string.h"

namespace rex
{
    enum class SID
    {
        // Special zero value, meaning no name.
        None,

        // Special constant for the last hard-coded name index
        MaxHardcodedNameIndex,
    };

    namespace conversions
    {
        rsl::string to_display_string(const SID& name);
    }
}