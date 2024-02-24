#pragma once

#include "rex_engine/diagnostics/error.h"
#include "rex_engine/memory/blob_view.h"
#include "rex_std/string_view.h"
#include "rex_std/vector.h"
#include "rex_std/bonus/utility/key_value.h"

namespace rex
{
  // This holds a header (with or without a name)
  // and its internal items which are just key - value pairs
  class IniHeaderWithItems
  {
  public:
    IniHeaderWithItems(rsl::string_view header, rsl::vector<rsl::key_value<rsl::string_view, rsl::string_view>>&& items);

    rsl::string_view header() const;
    const rsl::vector<rsl::key_value<rsl::string_view, rsl::string_view>>& items() const;

  private:
    rsl::string_view m_header;
    rsl::vector<rsl::key_value<rsl::string_view, rsl::string_view>> m_items;
  };

  // A processor taking in data, but doesn't own it
  // It just parses the data and returns a result.
  // The parsed data can be accessed using the getter.
  class IniProcessor
  {
  public:
    IniProcessor(memory::BlobView data);
    
    rex::Error process();

    const rsl::vector<IniHeaderWithItems>& items() const;

  private:
    void add_new_header_with_items(rsl::string_view header, rsl::vector<rsl::key_value<rsl::string_view, rsl::string_view>>&& items);

  private:
    memory::BlobView m_data;
    rsl::vector<IniHeaderWithItems> m_headers_with_items;
  };
}