#include "rex_engine/text_processing/ini.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/text_processing/text_processing.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_std/string_view.h"

namespace rex
{
	namespace ini
	{
		IniBlock::IniBlock(rsl::string_view header, const rsl::vector<rsl::key_value<rsl::string_view, rsl::string_view>>& items)
			: m_header(header)
		{
			for (const auto& item : items)
			{
				m_items[item.key] = item.value;
			}
		}

		rsl::string_view IniBlock::header() const
		{
			return m_header;
		}
		rsl::string_view IniBlock::get(rsl::string_view key, rsl::string_view def) const
		{
			if (m_items.contains(key))
			{
				return m_items.at(key);
			}

			return def;
		}

		const rsl::unordered_map<rsl::string_view, rsl::string_view>& IniBlock::all_items() const
		{
			return m_items;
		}

		Ini::Ini(rsl::unordered_map<rsl::string_view, IniBlock>&& headerWithItems)
			: m_headers_with_items(headerWithItems)
			, m_parse_error(Error::no_error())
		{}
		Ini::Ini(Error parseError)
			: m_headers_with_items()
			, m_parse_error(parseError)
		{}

		rsl::string_view Ini::get(rsl::string_view header, rsl::string_view key, rsl::string_view def) const
		{
			if (m_headers_with_items.contains(header))
			{
				return m_headers_with_items.at(header).get(key, def);
			}

			return def;
		}

		bool Ini::is_discarded() const
		{
			return m_parse_error.has_error();
		}

		Error Ini::parse_error() const
		{
			return m_parse_error;
		}

		rsl::vector<IniBlock> Ini::all_blocks() const
		{
			rsl::vector<IniBlock> blocks;

			for (const auto& [key, block] : m_headers_with_items)
			{
				blocks.emplace_back(block);
			}

			return blocks;
		}

		Ini parse(rsl::string_view iniContent)
		{
			const rsl::vector<rsl::string_view> lines = rsl::split(iniContent, rex::endline());

			rsl::string_view current_header;
			rsl::vector<rsl::key_value<rsl::string_view, rsl::string_view>> items;
			rex::Error error = rex::Error::no_error();
			rsl::unordered_map<rsl::string_view, IniBlock> headers_with_items;

			auto add_new_header_with_items = [&headers_with_items](rsl::string_view header, const rsl::vector<rsl::key_value<rsl::string_view, rsl::string_view>>& items)
			{
				if (!items.empty())
				{
					headers_with_items.emplace(header, IniBlock(header, items));
				}
			};

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
						return Ini(rex::Error(rsl::format("Invalid line: \"{}\". No key provided", line)));
					}

					// an empty value is not support
					if (value.empty())
					{
						return Ini(rex::Error(rsl::format("Invalid line: \"{}\". No value provided", line)));
					}

					value = rex::remove_quotes(value);
					items.emplace_back(key, value);
					continue;
				}

				// anything else is an error
				return Ini(rex::Error(rsl::format("Invalid line: \"{}\"", line)));
			}

			add_new_header_with_items(current_header, items);
			return Ini(rsl::move(headers_with_items));
		}

		Ini read_from_file(rsl::string_view filepath)
		{
			if (!rex::vfs::is_file(filepath))
			{
				return Ini(Error("File does not exist"));
			}

			rex::memory::Blob file_content = rex::vfs::read_file(filepath);
			return rex::ini::parse(rex::memory::blob_to_string_view(file_content));
		}
	}


} // namespace rex