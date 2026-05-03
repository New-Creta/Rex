#pragma once

#include "rex_engine/engine/globals.h"
#include "rex_engine/text_processing/json.h"

#include "rex_std/vector.h"
#include "rex_std/string_view.h"
#include "rex_std/string.h"

namespace rex
{
	// A module represent a single Visual Studio project or its equivalent
	// Having this detected at runtime allows us to find what the main module is we're running
	// which is used to determine the log file and other session data
	class Module
	{
	public:
		Module(rsl::string_view moduleName, rsl::string_view configName, rsl::string_view sourceRoot, rsl::string_view dataRoot, rsl::string_view targetPath, rsl::vector<Module*>&& dependencies);
		const rsl::vector<Module*>& dependencies() const;

		rsl::string_view name() const;
		rsl::string_view data_path() const;
		rsl::string_view target_path() const;

		void add_runtime_dependency(Module* module);

		// Find a runtime module of a plugin
		// This allows a client to load the runtime module
		// or run in it, depending if it's a dll or an executable
		const Module* find_runtime_dependency(rsl::string_view name) const;

	private:
		rsl::string m_name;
		rsl::string m_config_name;
		rsl::string m_source_root;
		rsl::string m_data_path;
		rsl::string m_target_path;
		rsl::vector<Module*> m_dependencies;
		rsl::vector<Module*> m_runtime_dependencies;
	};
}