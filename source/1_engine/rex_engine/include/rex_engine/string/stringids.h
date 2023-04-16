#pragma once

#include <assert.h>
#include <string>

namespace sbt
{
    // Define a message as an enumeration.
#define REGISTER_NAME(num,name) SID_##name = num,
    enum class EName 
    {
        // Special zero value, meaning no name.
        REGISTER_NAME(0,None)

        // Special constant for the last hard-coded name index
        SID_MaxHardcodedNameIndex,
    };
    #undef REGISTER_NAME

    namespace conversions
    {
        std::string toDisplayString(const EName& name);
    }
}