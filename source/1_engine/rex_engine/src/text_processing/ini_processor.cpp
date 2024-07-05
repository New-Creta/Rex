#include "rex_engine/text_processing/ini_processor.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/text_processing/text_processing.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_std/string_view.h"

namespace rex
{
  IniHeaderWithItems::IniHeaderWithItems(rsl::string_view header, const rsl::vector<rsl::key_value<rsl::string_view, rsl::string_view>>& items)
    : m_header(header)
  {
    for (const auto& item : items)
    {
      m_items[item.key] = item.value;
    }
  }

  rsl::string_view IniHeaderWithItems::header() const
  {
    return m_header;
  }
  rsl::string_view IniHeaderWithItems::get(rsl::string_view key, rsl::string_view def) const
  {
    if (m_items.contains(key))
    {
      return m_items.at(key);
    }

    return def;
  }

  const rsl::unordered_map<rsl::string_view, rsl::string_view>& IniHeaderWithItems::all_items() const
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

    for (rsl::string_view line : lines)
    {
      // Make sure we remove all leading and trailing whitespaces
      line = rex::strip(line);

      // Headers are surrounded with '[' and ']'
      if (line.starts_with('[') && line.ends_with(']'))
      {
        add_new_header_with_items(current_header, items);
        items.clear();
        current_header = line.substr(1, line.length() - 2);
        continue;
      }

      // an item looks like key=value
      auto equal_pos = line.find('=');
      if (equal_pos != line.npos()) // NOLINT(readability-static-accessed-through-instance)
      {
        rsl::string_view key = line.substr(0, equal_pos);
        rsl::string_view value = line.substr(equal_pos + 1);

        // an empty key is not support
        if (key.empty())
        {
          m_headers_with_items.clear();
          return rex::Error(rsl::format("Invalid line: \"{}\". No key provided ({})", line, m_filepath));
        }

        // an empty value is not support
        if (value.empty())
        {
          m_headers_with_items.clear();
          return rex::Error(rsl::format("Invalid line: \"{}\". No value provided ({})", line, m_filepath));
        }

        value = rex::remove_quotes(value);
        items.emplace_back(key, value);
        continue;
      }

      // anything else is an error
      m_headers_with_items.clear();
      return rex::Error(rsl::format("Invalid line: \"{}\" ({})", line, m_filepath));
    }

    add_new_header_with_items(current_header, items);
    return error;
  }

  rsl::string_view IniProcessor::get(rsl::string_view header, rsl::string_view key, rsl::string_view def) const
  {
    if (m_headers_with_items.contains(header))
    {
      return m_headers_with_items.at(header).get(key, def);
    }

    return def;
  }

  rsl::unordered_map<rsl::string_view, IniHeaderWithItems> IniProcessor::all_items() const
  {
    return m_headers_with_items;
  }

  void IniProcessor::add_new_header_with_items(rsl::string_view header, const rsl::vector<rsl::key_value<rsl::string_view, rsl::string_view>>&items)
  {
    if (!items.empty())
    {
      m_headers_with_items.emplace(header, IniHeaderWithItems(header, items));
    }
  }

} // namespace rex