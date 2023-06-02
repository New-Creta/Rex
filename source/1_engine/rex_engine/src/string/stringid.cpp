#include "rex_engine/string/stringid.h"

#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/log.h"
#include "rex_std/bonus/types.h"

namespace rex
{
  namespace internal
  {
    //-------------------------------------------------------------------------
    StringEntryID make_and_store(rsl::string_view characters)
    {
      if(characters.empty())
      {
        REX_WARN(LogEngine, "Trying to create a StringID with an empty string, this is not allowed.");
        return StringEntryID(); // StringID::is_none() == true
      }

      return string_pool::make_and_store(characters);
    }
  } // namespace internal

  //-------------------------------------------------------------------------
  StringID store_sid(rsl::string_view characters)
  {
    return StringID(string_pool::make_and_store(characters));
  }

  //-------------------------------------------------------------------------
  bool operator==(rsl::string_view s, const StringID& sid)
  {
    return StringID(s) == sid;
  }
  //-------------------------------------------------------------------------
  bool operator!=(rsl::string_view s, const StringID& sid)
  {
    return StringID(s) != sid;
  }
  //-------------------------------------------------------------------------
  bool operator==(const StringID& sid, rsl::string_view s)
  {
    return s == sid;
  }
  //-------------------------------------------------------------------------
  bool operator!=(const StringID& sid, rsl::string_view s)
  {
    return s != sid;
  }
} // namespace rex

//-------------------------------------------------------------------------
rsl::ostream& operator<<(rsl::ostream& os, const rex::StringID& stringID)
{
  os << stringID.to_string_view();

  return os;
}