#include "rex_engine/text_processing/json.h"

#define JSON_NOEXCEPTION
#include "rex_engine/text_processing/internal/json.hpp"

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
	}
}