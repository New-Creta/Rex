#pragma once

#include "rex_std/string.h"

#include "rex_engine/memory/memory_types.h"





namespace rex
{
	class NinjaProject
	{

	};

	class NinjaSolution
	{
	public:
		NinjaSolution(rsl::string_view solutionPath);

		static scratch_string find_from_path(rsl::string_view path);

	private:
		void init_root_project();

	private:
		rsl::string m_solution_path;
		rsl::unique_ptr<NinjaProject> m_root_project;
	};
}