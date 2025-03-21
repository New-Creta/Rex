#pragma once

#include "rex_engine/engine/casting.h"
#include "rex_engine/diagnostics/error.h"
#include "rex_engine/memory/blob_view.h"
#include "rex_std/bonus/utility/key_value.h"
#include "rex_std/string_view.h"
#include "rex_std/vector.h"
#include "rex_std/unordered_map.h"

namespace rex
{
  namespace ini
  {
		// This holds a header (with or without a name)
		// and its internal items which are just key - value pairs
    template <typename Allocator = rsl::allocator>
		class IniBlock
		{
		public:
			IniBlock(rsl::string_view header, const rsl::vector<rsl::key_value<rsl::string_view, rsl::string_view>, Allocator>& items);

			rsl::string_view header() const;
			rsl::string_view get(rsl::string_view key, rsl::string_view def = "") const;
			const rsl::unordered_map<rsl::string_view, rsl::string_view>& all_items() const;

		private:
			rsl::string_view m_header;
			rsl::unordered_map<
        rsl::string_view,                 // key
        rsl::string_view,                 // value
        rsl::hash<rsl::string_view>,      // hash
        rsl::equal_to<rsl::string_view>,  // comparison
        Allocator> m_items;
		};

    template <typename Allocator = rsl::allocator>
    class TIni
    {
    public:
      // Construct an ini object with ini content
      explicit TIni(rsl::unordered_map<rsl::string_view, IniBlock<Allocator>>&& headerWithItems);
      explicit TIni(Error parseError);

      // Return the value of a key, possible within a header
      rsl::string_view get(rsl::string_view header, rsl::string_view key, rsl::string_view def = "") const;

      // Return if the content is discard aka, the content is invalid and a parse error occurred
      bool is_discarded() const;

      // Return the parse error
      Error parse_error() const;

      // Return all items within this ini object
			rsl::vector<IniBlock<Allocator>, Allocator> all_blocks() const;

    private:
      rsl::unordered_map<
        rsl::string_view,                 // key
        IniBlock<Allocator>,              // value
        rsl::hash<rsl::string_view>,      // hash
        rsl::equal_to<rsl::string_view>,  // comparison
        Allocator> m_headers_with_items;
      rex::Error m_parse_error;
    };

    using Ini = TIni<rsl::allocator>;

    template <typename Allocator = rsl::allocator>
    TIni<Allocator> parse(rsl::string_view filepath);
    template <typename Allocator = rsl::allocator>
    TIni<Allocator> read_from_file(rsl::string_view filepath);
  }
} // namespace rex

#include "rex_engine/text_processing/ini.template.h"