#pragma once

#define JSON_NOEXCEPTION
#include "rex_engine/json/internal/json.hpp"

#include "rex_engine/memory/blob.h"
#include "rex_engine/memory/blob_view.h"

namespace rex
{
	namespace json
	{
		using namespace nlohmann;

		auto parse(memory::BlobView blob)
		{
			auto start = blob.data();
			auto end = blob.data() + blob.size();
			return nlohmann::json::parse(start, end, nullptr, false);
		}
	}
}