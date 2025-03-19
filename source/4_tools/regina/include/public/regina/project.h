#pragma once

#include "rex_std/string_view.h"
#include "rex_std/string.h"

#include "rex_engine/text_processing/json.h"

namespace regina
{
	class Project
	{
	public:
		Project(rsl::string_view filepath);

		rsl::string_view name() const;

	private:
		void parse_json(const rex::json::json& projectJson);

	private:
		rsl::string m_name;
	};
}