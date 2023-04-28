#include "rex_engine/string/stringpool.h"

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/string/stringentry.h"
#include "rex_std/functional.h"
#include "rex_std/internal/utility/pair.h"
#include "rex_std/unordered_map.h"

namespace rsl
{
  inline namespace v1
  {
    // https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
    rsl::hash_result hash_bytes(const void* data, rsl::count_t size) noexcept
    {
      const rsl::hash_result fnv_offset_basis = 2166136261u;
      const rsl::hash_result fnv_prime        = 16777619u;

      const u8* bytes = static_cast<const u8*>(data);

      rsl::hash_result hash = fnv_offset_basis;
      for(rsl::count_t i = 0; i < size; ++i)
      {
        hash ^= bytes[i];
        hash *= fnv_prime;
      }

      return hash;
    }

    // Template specialization for hash a rsl::string
    template <>
    struct hash<rsl::string>
    {
      rsl::hash_result operator()(const rsl::string& s) const noexcept
      {
        return hash_bytes(s.data(), s.size());
      }
    };

    // Template specialization for hash a const char*
    template <>
    struct hash<const char*>
    {
      rsl::hash_result operator()(const char* s) const noexcept
      {
        return hash_bytes(s, rsl::strlen(s));
      }
    };
  } // namespace v1
} // namespace rsl

namespace rex
{
  namespace string_pool
  {
    using EntryMap = rsl::unordered_map<StringEntryID, StringEntry>;

    //-------------------------------------------------------------------------
    EntryMap load_entry_map()
    {
      EntryMap map;

      for(u32 i = 0; i < static_cast<u32>(SID::MaxHardcodedNameIndex); ++i)
      {
        rsl::string sid_name = conversions::to_display_string(static_cast<SID>(i));

        StringEntryID entry_id(static_cast<rsl::hash_result>(i));
        StringEntry entry(sid_name.data(), sid_name.size());

        map.emplace(rsl::move(entry_id), rsl::move(entry));
      }

      return map;
    }

    //-------------------------------------------------------------------------
    EntryMap& get_entries()
    {
      static EntryMap entries = load_entry_map();
      return entries;
    }

    //-------------------------------------------------------------------------
    const StringEntryID* store(rsl::hash_result hash, const char* characters, s32 size)
    {
      auto it = get_entries().find(hash);
      if(it != rsl::cend(get_entries()))
      {
        REX_ASSERT_X(rsl::strcmp(characters, it->value.get_characters()) == 0, "Hash collision");

        return &it->key;
      }

      StringEntryID entry_id(hash);
      StringEntry entry(characters, size);

      auto result = get_entries().emplace(rsl::move(entry_id), rsl::move(entry));
      if(result.emplace_successful)
      {
        return &result.inserted_element->key;
      }

      return nullptr;
    }

    //-------------------------------------------------------------------------
    void resolve(const StringEntryID& entryID, const char** out, s32& outSize)
    {
      const StringEntry* entry = find(entryID);

      REX_ASSERT_X(entry != nullptr, "Entry not found");

      entry->get_characters(out, outSize);
    }
    //-------------------------------------------------------------------------
    void resolve(const StringEntryID& entryID, rsl::string& out)
    {
      const StringEntry* entry = find(entryID);

      REX_ASSERT_X(entry != nullptr, "Entry not found");

      out = rsl::string(entry->get_characters(), entry->get_size());
    }

    //-------------------------------------------------------------------------
    const StringEntry* find(const StringEntryID& entryID)
    {
      auto it = get_entries().find(entryID);
      if(it == rsl::cend(get_entries()))
      {
        const rsl::hash_result sid_name = static_cast<rsl::hash_result>(SID::None);

        it = get_entries().find(sid_name);

        REX_ASSERT_X(it != rsl::cend(get_entries()), "SID::None not present");
      }

      return &(it->value);
    }

    //-------------------------------------------------------------------------
    const StringEntryID* store(const SID& name)
    {
      rsl::string sid_name = conversions::to_display_string(name);

      return store(static_cast<rsl::hash_result>(name), sid_name.data(), sid_name.size());
    }
    //-------------------------------------------------------------------------
    const StringEntryID* store(const char* characters, s32 size)
    {
      return store(rsl::hash<const char*> {}(characters), characters, size);
    }
  } // namespace string_pool
} // namespace rex