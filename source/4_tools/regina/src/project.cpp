#include "regina/project.h"

#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/diagnostics/assert.h"

namespace regina
{
	Project::Project(rsl::string_view name)
		: m_name(name)
	{
	}

	rsl::string_view Project::name() const
	{
		return m_name;
	}

}