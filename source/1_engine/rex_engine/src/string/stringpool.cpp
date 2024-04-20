#include "rex_engine/string/stringpool.h"

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/memory/memory_tags.h"
#include "rex_engine/memory/memory_tracking.h"
#include "rex_engine/string/stringid.h"
#include "rex_std/bonus/functional.h"
#include "rex_std/bonus/hashtable.h"
#include "rex_std/bonus/utility.h"
#include "rex_std/cstring.h"
#include "rex_std/unordered_map.h"

namespace rex
{
  namespace string_pool
  {
    using EntryMap = rsl::unordered_map<rsl::hash_result, rsl::string>;

    //-------------------------------------------------------------------------
    EntryMap& get_entries()
    {
      static EntryMap entries;
      return entries;
    }

    //-------------------------------------------------------------------------
    StringID find_or_store(rsl::string_view string)
    {
      rsl::hash_result hash = rsl::hash<rsl::string_view>{}(string);
      REX_MEM_TAG_SCOPE(MemoryTag::StringPool);

      // Check if the string isn't already in the string pool
      auto it = get_entries().find(hash);
      if (it != rsl::cend(get_entries()))
      {
        // If we have found an existing entry, return it
        return StringID(hash, it->value);
      }

      // If we haven't found an existing entry, create a new entry
      auto res = get_entries().emplace(hash, rsl::string(string));
      if (res.emplace_successful)
      {
        return StringID(hash, res.inserted_element->value);
      }

      REX_ASSERT("This path should never be reached, insertion into the string pool failed somehow.");
      return StringID::create_invalid();
    }
  } // namespace string_pool
} // namespace rex