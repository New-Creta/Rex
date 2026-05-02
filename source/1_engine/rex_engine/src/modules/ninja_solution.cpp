#include "rex_engine/modules/ninja_solution.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/engine/module_manager.h"

namespace rex
{
	// Discover the "ninja" solution to load all available modules
	// Load the ninja project that represents the executing assembly
	// From that, load the dependencies

	NinjaSolution::NinjaSolution(rsl::string_view solutionPath)
		: m_solution_path(solutionPath)
	{
		init_root_project();
	}

	void NinjaSolution::init_root_project()
	{
		rsl::string_view root_project_name = module_manager::instance()->current()->name();
	}

	scratch_string NinjaSolution::find_from_path(rsl::string_view path)
	{
		return rex::path::find_in_parent(".nsln", path);
	}

}

