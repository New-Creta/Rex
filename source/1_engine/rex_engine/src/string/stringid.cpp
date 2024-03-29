#include "rex_engine/string/stringid.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/string/stringpool.h"

namespace rex
{
  namespace internal
  {
    //-------------------------------------------------------------------------
    StringID make_and_store(rsl::string_view characters)
    {
      if(characters.empty())
      {
        REX_WARN(LogEngine, "Trying to create a StringID with an empty string, this is not allowed.");
        return StringID::create_invalid();
      }

      return string_pool::make_and_store(characters);
    }
  } // namespace internal

  //-------------------------------------------------------------------------
  StringID store_sid(rsl::string_view characters)
  {
    return string_pool::make_and_store(characters);
  }
  //-------------------------------------------------------------------------
  rsl::string_view restore_sid(const StringID& sid)
  {
    return string_pool::resolve(sid);
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
  os << rex::restore_sid(stringID);

  return os;
}