#include "rex_engine/string/stringpool.h"

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/string/stringentry.h"
#include "rex_std/bonus/functional.h"
#include "rex_std/bonus/hashtable.h"
#include "rex_std/bonus/utility.h"
#include "rex_std/cstring.h"
#include "rex_std/internal/utility/pair.h"
#include "rex_std/unordered_map.h"

namespace rex
{
  namespace string_pool
  {
    using EntryMap = rsl::unordered_map<StringEntryID, StringEntry>;

    //-------------------------------------------------------------------------
    EntryMap load_entry_map()
    {
      EntryMap map;

      StringEntryID entry_id(StringEntryID::create_invalid());
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
    StringEntryID store(rsl::hash_result hash, rsl::string_view newCharacters)
    {
      StringEntryID entry_id = StringEntryID(hash);

      StringEntry entry(newCharacters);
      auto it = get_entries().find(entry_id);
      if(it != rsl::cend(get_entries()))
      {
        REX_ASSERT_X(rsl::strcmp(newCharacters.data(), it->value.characters().data()) == 0, "Hash collision");

        return it->key;
      }

      auto result = get_entries().emplace(rsl::move(entry_id), rsl::move(entry));
      if(result.emplace_successful)
      {
        return result.inserted_element->key;
      }

      REX_ASSERT("This path should never be reached, insertion into the string pool failed somehow.");
      return {};
    }

    //-------------------------------------------------------------------------
    rsl::string_view resolve(const StringEntryID& entryID)
    {
      const StringEntry& entry = find(entryID);

      REX_ASSERT_X(entry.is_valid(), "Entry not found");

      return entry.characters();
    }

    //-------------------------------------------------------------------------
    const StringEntry& find(const StringEntryID& entryID)
    {
      auto it = get_entries().find(entryID);
      if(it == rsl::cend(get_entries()))
      {
        it = get_entries().find(StringEntryID::create_invalid());

        REX_ASSERT_X(it != rsl::cend(get_entries()), "StringID::is_none() not present");
      }

      return it->value;
    }

    //-------------------------------------------------------------------------
    StringEntryID make_and_store(rsl::string_view characters)
    {
      return store(rsl::hash<rsl::string_view> {}(characters), characters);
    }
  } // namespace string_pool
} // namespace rex