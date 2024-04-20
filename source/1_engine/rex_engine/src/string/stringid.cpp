#include "rex_engine/string/stringid.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/string/stringpool.h"

namespace rex
{
  StringID StringID::create(rsl::string_view string)
  {
    return string_pool::find_or_store(string);
  }
} // namespace rex

//-------------------------------------------------------------------------
rsl::ostream& operator<<(rsl::ostream& os, const rex::StringID& stringID)
{
  os << stringID.string();

  return os;
}