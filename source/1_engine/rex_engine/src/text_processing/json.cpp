#include "rex_engine/text_processing/json.h"

#define JSON_NOEXCEPTION
#include "rex_engine/text_processing/internal/json.hpp"

#include "rex_engine/filesystem/vfs.h"

namespace rex
{
	namespace json
	{
		json::json parse(memory::BlobView blob)
		{
			auto start = blob.data();
			auto end = blob.data() + blob.size();
			return nlohmann::json::parse(start, end, nullptr, false);
		}

		json::json read_from_file(rsl::string_view filepath)
		{
			rex::memory::Blob file_content = rex::vfs::read_file(filepath);
			return rex::json::parse(file_content);
		}
	}
}