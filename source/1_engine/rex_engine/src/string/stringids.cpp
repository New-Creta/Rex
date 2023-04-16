#include "string/stringids.h"

namespace sbt
{
    namespace conversions
    {
        #define SID_NAME(name) EName::SID_##name: return #name
        //-------------------------------------------------------------------------
        std::string toDisplayString(const EName& name)
        {
            switch(name)
            {
                case SID_NAME(None);
                default:
                    assert(false && "Invalid name given!");
                    return "None";
            }
        }
        #undef SID_NAME
    }
}