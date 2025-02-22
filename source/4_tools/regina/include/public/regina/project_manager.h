#pragma once

#include "rex_std/string_view.h"
#include "rex_std/memory.h"

#include "regina/project.h"

namespace regina
{
	namespace project_manager
	{
		rsl::unique_ptr<Project> create_new(rsl::string_view projectName);
		rsl::unique_ptr<Project> load_from_disk(rsl::string_view projectName);
	}
}
