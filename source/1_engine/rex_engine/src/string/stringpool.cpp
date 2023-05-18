#include "rex_engine/string/stringpool.h"

#include "rex_engine/diagnostics/assert.h" // for REX_ASSERT_X, REX_ASSERT
#include "rex_engine/string/stringentry.h" // for StringEntryID, StringEntry
#include "rex_std/bonus/functional.h"      // for hash_result
#include "rex_std/bonus/hashtable.h"       // for hashtable_iterator, opera...
#include "rex_std/bonus/types.h"           // for char8
#include "rex_std/bonus/utility.h"         // for key_value, emplace_result
#include "rex_std/cstring.h"
#include "rex_std/internal/utility/pair.h"
#include "rex_std/unordered_map.h" // for unordered_map

namespace rex
{
  namespace string_pool
  {
    using EntryMap = rsl::unordered_map<StringEntryID, StringEntry>;

    //-------------------------------------------------------------------------
    EntryMap load_entry_map()
    {
      EntryMap map;

      StringEntryID entry_id(StringEntryID::s_none_state_hash);
      StringEntry entry("Invalid StringID");

      map.emplace(rsl::move(entry_id), rsl::move(entry));

      return map;
    }

    //-------------------------------------------------------------------------
    EntryMap& get_entries()
    {
      static EntryMap entries = load_entry_map();
      return entries;
    }

    //-------------------------------------------------------------------------
    const StringEntryID* store(rsl::hash_result hash, rsl::string_view characters)
    {
      auto it = get_entries().find(hash);
      if(it != rsl::cend(get_entries()))
      {
        REX_ASSERT_X(rsl::strcmp(characters.data(), it->value.characters().data()) == 0, "Hash collision");

        return &it->key;
      }

      StringEntryID entry_id(hash);
      StringEntry entry(characters);

      auto result = get_entries().emplace(rsl::move(entry_id), rsl::move(entry));
      if(result.emplace_successful)
      {
        return &result.inserted_element->key;
      }

      REX_ASSERT("This path should never be reached, insertion into the string pool failed somehow.");
      return nullptr;
    }

    //-------------------------------------------------------------------------
    rsl::string_view resolve(const StringEntryID& entryID)
    {
      const StringEntry* entry = find(entryID);

      REX_ASSERT_X(entry != nullptr, "Entry not found");

      return entry->characters();
    }

    //-------------------------------------------------------------------------
    const StringEntry* find(const StringEntryID& entryID)
    {
      auto it = get_entries().find(entryID);
      if(it == rsl::cend(get_entries()))
      {
        it = get_entries().find(StringEntryID::s_none_state_hash);

        REX_ASSERT_X(it != rsl::cend(get_entries()), "StringID::is_none() not present");
      }

      return &(it->value);
    }

    //-------------------------------------------------------------------------
    const StringEntryID* store(rsl::string_view characters)
    {
      return store(rsl::hash<rsl::string_view> {}(characters), characters);
    }
  } // namespace string_pool
} // namespace rex