#include "rex_engine/string/stringids.h"
#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  namespace conversions
  {
    //-------------------------------------------------------------------------
    rsl::string to_display_string(const SID& name)
    {
      switch(name)
      {
        case SID::None: return rsl::string("None");
        default: REX_ASSERT("Invalid SID given!"); 
            return rsl::string("None");
      }
    }
  } // namespace conversions
} // namespace rex