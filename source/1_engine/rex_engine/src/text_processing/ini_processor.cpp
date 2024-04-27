#include "rex_engine/text_processing/ini_processor.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/text_processing/text_processing.h"
#include "rex_std/string_view.h"

namespace rex
{
  IniHeaderWithItems::IniHeaderWithItems(rsl::string_view header, rsl::vector<rsl::key_value<rsl::string_view, rsl::string_view>>&& items)
      : m_header(header)
      , m_items(rsl::move(items))
  {
  }

  rsl::string_view IniHeaderWithItems::header() const
  {
    return m_header;
  }
  const rsl::vector<rsl::key_value<rsl::string_view, rsl::string_view>>& IniHeaderWithItems::items() const
  {
    return m_items;
  }

  IniProcessor::IniProcessor(memory::BlobView data)
      : m_data(data)
  {
  }

  rex::Error IniProcessor::process()
  {
    const rsl::string_view content(m_data.data_as<const char>(), static_cast<s32>(m_data.size()));
    const rsl::vector<rsl::string_view> lines = rsl::split(content, rex::endline());

    rsl::string_view current_header;
    rsl::vector<rsl::key_value<rsl::string_view, rsl::string_view>> items;
    rex::Error error = rex::Error::no_error();

    for(rsl::string_view line: lines)
    {
      // Make sure we remove all leading and trailing whitespaces
      line = rex::strip(line);

      // Headers are surrounded with '[' and ']'
      if(line.starts_with('[') && line.ends_with(']'))
      {
        add_new_header_with_items(current_header, rsl::move(items));
        current_header = line.substr(1, line.length() - 2);
        continue;
      }

      // an item looks like key=value
      auto equal_pos = line.find('=');
      if(equal_pos != line.npos()) // NOLINT(readability-static-accessed-through-instance)
      {
        rsl::string_view key   = line.substr(0, equal_pos);
        rsl::string_view value = line.substr(equal_pos + 1);
        
        // an empty key is not support
        if (key.empty())
        {
          m_headers_with_items.clear();
          return rex::Error(rsl::format("Invalid line: \"{}\". No key provided", line));
        }

        // an empty value is not support
        if (value.empty())
        {
          m_headers_with_items.clear();
          return rex::Error(rsl::format("Invalid line: \"{}\". No value provided", line));
        }

        value = rex::remove_quotes(value);
        items.emplace_back(key, value);
        continue;
      }

      // anything else is an error
      m_headers_with_items.clear();
      return rex::Error(rsl::format("Invalid line: \"{}\"", line));
    }

    add_new_header_with_items(current_header, rsl::move(items));
    return error;
  }

  const rsl::vector<IniHeaderWithItems>& IniProcessor::items() const
  {
    return m_headers_with_items;
  }

  void IniProcessor::add_new_header_with_items(rsl::string_view header, rsl::vector<rsl::key_value<rsl::string_view, rsl::string_view>>&& items)
  {
    if(!items.empty())
    {
      m_headers_with_items.emplace_back(header, rsl::move(items));
    }
  }

} // namespace rex