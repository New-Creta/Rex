#pragma once

#define JSON_NOEXCEPTION
#include "rex_engine/text_processing/internal/json.hpp"

#include "rex_engine/memory/blob.h"
#include "rex_engine/memory/blob_view.h"

namespace rex
{
	namespace json
	{
		using namespace nlohmann;

		json::json parse(memory::BlobView blob);
		json::json read_from_file(rsl::string_view filepath);
	}
}