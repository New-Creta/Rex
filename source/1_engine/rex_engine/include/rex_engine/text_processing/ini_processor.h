#pragma once

#include "rex_engine/diagnostics/error.h"
#include "rex_engine/memory/blob_view.h"
#include "rex_std/string_view.h"
#include "rex_std/vector.h"
#include "rex_std/bonus/utility/key_value.h"

namespace rex
{
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

  class IniProcessor
  {
  public:
    IniProcessor(memory::BlobView data);
    
    Error process();

    const rsl::vector<IniHeaderWithItems>& items() const;

  private:
    void add_new_header_with_items(rsl::string_view header, rsl::vector<rsl::key_value<rsl::string_view, rsl::string_view>>&& items);

  private:
    memory::BlobView m_data;
    rsl::vector<IniHeaderWithItems> m_headers_with_items;
  };
}