#pragma once

#include "rex_engine/diagnostics/error.h"
#include "rex_engine/memory/blob_view.h"
#include "rex_std/bonus/utility/key_value.h"
#include "rex_std/string_view.h"
#include "rex_std/vector.h"
#include "rex_std/unordered_map.h"

namespace rex
{
  // This holds a header (with or without a name)
  // and its internal items which are just key - value pairs
  class IniHeaderWithItems
  {
  public:
    IniHeaderWithItems(rsl::string_view header, const rsl::vector<rsl::key_value<rsl::string_view, rsl::string_view>>& items);

    rsl::string_view header() const;
    rsl::string_view get(rsl::string_view key, rsl::string_view def = "") const;
    const rsl::unordered_map<rsl::string_view, rsl::string_view>& all_items() const;

  private:
    rsl::string_view m_header;
    rsl::unordered_map<rsl::string_view, rsl::string_view> m_items;
  };

  // A processor taking in data, but doesn't own it
  // It just parses the data and returns a result.
  // The parsed data can be accessed using the getter.
  class IniProcessor
  {
  public:
    explicit IniProcessor(memory::BlobView data);

    rex::Error process();

    rsl::string_view get(rsl::string_view header, rsl::string_view key, rsl::string_view def = "") const;
    rsl::unordered_map<rsl::string_view, IniHeaderWithItems> all_items() const;

  private:
    void add_new_header_with_items(rsl::string_view header, const rsl::vector<rsl::key_value<rsl::string_view, rsl::string_view>>& items);

  private:
    memory::BlobView m_data;
    rsl::string_view m_filepath;
    rsl::unordered_map<rsl::string_view, IniHeaderWithItems> m_headers_with_items;
  };
} // namespace rex