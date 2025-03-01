#pragma once

#include "regina/thumbnail.h"

#include "rex_std/string_view.h"
#include "rex_std/string.h"
#include "rex_std/unordered_map.h"
#include "rex_std/memory.h"

namespace regina
{
	class ThumbnailManager
	{
	public:
		ThumbnailManager();

		bool has_thumbnail(rsl::string_view path) const;
		const Thumbnail* thumbnail_for_path(rsl::string_view path) const;

		const Thumbnail* directory_thumbnail() const;
		const Thumbnail* file_thumbnail() const;
		const Thumbnail* unknown_thumbnail() const;

	private:
		rsl::unique_ptr<Thumbnail> create_thumbnail_from_path(rsl::string_view path);

	private:
		rsl::unordered_map<rsl::string, rsl::unique_ptr<Thumbnail>> m_extension_to_thumbnail;

		rsl::unique_ptr<Thumbnail> m_directory_thumbnail;
		rsl::unique_ptr<Thumbnail> m_file_thumbnail;
		rsl::unique_ptr<Thumbnail> m_unknown_thumbnail;

	};
}