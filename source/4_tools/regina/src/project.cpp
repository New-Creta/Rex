#include "regina/project.h"

#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/diagnostics/assert.h"

namespace regina
{
	Project::Project(rsl::string_view filepath)
	{
		REX_ASSERT_X(rex::vfs::is_file(filepath), "A project filepath given to the project class should point to a path that exists. Path: \"{}\"", filepath);

		auto project_json = rex::json::read_from_file(filepath);
		parse_json(project_json);
	}

	rsl::string_view Project::name() const
	{
		return m_name;
	}

	void Project::parse_json(const rex::json::json& projectJson)
	{
		m_name = projectJson["name"];
	}

}