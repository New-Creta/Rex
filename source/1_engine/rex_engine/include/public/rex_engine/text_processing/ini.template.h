#pragma once

#include "rex_engine/text_processing/ini.h"

#include "rex_engine/memory/alloc_unique.h"
#include "rex_engine/text_processing/ini.h"
#include "rex_engine/engine/casting.h"
#include "rex_engine/diagnostics/log.h"
#include "rex_engine/text_processing/text_processing.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_std/string_view.h"

namespace rex
{
	namespace ini
	{
		// ----------------------
		// IniBlock
		// ----------------------
		template <typename Allocator>
		IniBlock<Allocator>::IniBlock(rsl::string_view header, const rsl::vector<rsl::key_value<rsl::string_view, rsl::string_view>, Allocator>& items)
			: m_header(header)
		{
			for (const auto& item : items)
			{
				m_items[item.key] = item.value;
			}
		}

		template <typename Allocator>
		rsl::string_view IniBlock<Allocator>::header() const
		{
			return m_header;
		}
		template <typename Allocator>
		rsl::string_view IniBlock<Allocator>::get(rsl::string_view key, rsl::string_view def) const
		{
			if (m_items.contains(key))
			{
				return m_items.at(key);
			}

			return def;
		}

		template <typename Allocator>
		const rsl::unordered_map<rsl::string_view, rsl::string_view>& IniBlock<Allocator>::all_items() const
		{
			return m_items;
		}

		// ----------------------
		// Ini
		// ----------------------

		template <typename Allocator>
		TIni<Allocator>::TIni(rsl::unordered_map<rsl::string_view, IniBlock<Allocator>>&& headerWithItems)
			: m_headers_with_items(headerWithItems)
			, m_parse_error(Error::no_error())
		{}
		template <typename Allocator>
		TIni<Allocator>::TIni(Error parseError)
			: m_headers_with_items()
			, m_parse_error(parseError)
		{}

		template <typename Allocator>
		rsl::string_view TIni<Allocator>::get(rsl::string_view header, rsl::string_view key, rsl::string_view def) const
		{
			if (m_headers_with_items.contains(header))
			{
				return m_headers_with_items.at(header).get(key, def);
			}

			return def;
		}

		template <typename Allocator>
		bool TIni<Allocator>::is_discarded() const
		{
			return m_parse_error.has_error();
		}

		template <typename Allocator>
		Error TIni<Allocator>::parse_error() const
		{
			return m_parse_error;
		}

		template <typename Allocator>
		rsl::vector<IniBlock<Allocator>, Allocator> TIni<Allocator>::all_blocks() const
		{
			rsl::vector<IniBlock<Allocator>, Allocator> blocks;

			for (const auto& [key, block] : m_headers_with_items)
			{
				blocks.emplace_back(block);
			}

			return blocks;
		}

		// ----------------------
		// Utility Functions
		// ----------------------

		template <typename Allocator>
		TIni<Allocator> parse(rsl::string_view iniContent)
		{
			const rsl::vector<rsl::string_view, Allocator> lines = rsl::split<Allocator>(iniContent, rex::endline());

			rsl::string_view current_header;
			rsl::vector<rsl::key_value<rsl::string_view, rsl::string_view>, Allocator> items;
			rex::Error error = rex::Error::no_error();
			rsl::unordered_map<
				rsl::string_view,                 // key
				IniBlock<Allocator>,              // value
				rsl::hash<rsl::string_view>,      // hash
				rsl::equal_to<rsl::string_view>,  // comparison
				Allocator> headers_with_items;

			auto add_new_header_with_items = [&headers_with_items](rsl::string_view header, const rsl::vector<rsl::key_value<rsl::string_view, rsl::string_view>, Allocator>& items)
				{
					if (!items.empty())
					{
						headers_with_items.emplace(header, IniBlock<Allocator>(header, items));
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
						return TIni<Allocator>(rex::Error(rsl::format("Invalid line: \"{}\". No key provided", line)));
					}

					// an empty value is not support
					if (value.empty())
					{
						return TIni<Allocator>(rex::Error(rsl::format("Invalid line: \"{}\". No value provided", line)));
					}

					value = rex::remove_quotes(value);
					items.emplace_back(key, value);
					continue;
				}

				// anything else is an error
				return TIni<Allocator>(rex::Error(rsl::format("Invalid line: \"{}\"", line)));
			}

			add_new_header_with_items(current_header, items);
			return TIni<Allocator>(rsl::move(headers_with_items));
		}

		template <typename Allocator>
		TIni<Allocator> read_from_file(rsl::string_view filepath)
		{
			if (!rex::vfs::is_file(filepath))
			{
				return TIni<Allocator>(Error("File does not exist"));
			}

			Allocator alloc;
			auto blob = alloc_unique<rsl::byte[]>(alloc, 1_kib);

			rex::vfs::read_file(filepath, blob.get(), blob.count());
			return rex::ini::parse<Allocator>(rsl::string_view(char_cast(blob.get()), blob.count()));
		}
	}
}

