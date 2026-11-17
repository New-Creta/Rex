#pragma once

#include "rex_hostfxr/hostfxr.h"
#include "rex_nethost/coreclr_delegates.h"

#include "rex_engine/system/library_loader.h"

namespace rex
{
	// The dotnet runtime is essentially a wrapper around the Hostfxr library
	// It handles initialization of the library
	class DotNetRuntime
	{
	public:
		DotNetRuntime();
		DotNetRuntime(const DotNetRuntime&) = delete;
		DotNetRuntime(DotNetRuntime&&) = delete;

		~DotNetRuntime();

		DotNetRuntime& operator=(const DotNetRuntime&) = delete;
		DotNetRuntime& operator=(DotNetRuntime&&) = delete;

		// Loads clr api function and return it
		load_assembly_and_get_function_pointer_fn load_get_function_delegate(rsl::string_view runtimeConfigPath);

	private:
		// Init hostfxr itself, finding its path and loading the module
		void init_hostfxr();
		// Calls into the hostfxr module to load the api functions
		void init_hostfxr_funcs();

	private:
		// The module to hostfxr itself
		library_handle m_module;

		// These are functions, part of the hostfxr.dll itself, which we're loading at runtime
		hostfxr_initialize_for_dotnet_command_line_fn m_init_for_cmd_line_fptr;
		hostfxr_initialize_for_runtime_config_fn m_init_for_config_fptr;
		hostfxr_get_runtime_delegate_fn m_get_delegate_fptr;
		hostfxr_run_app_fn m_run_app_fptr;
		hostfxr_close_fn m_close_fptr;

		// This is part of the Common Language Runtime (CLR)
		hostfxr_handle m_config_handle;
	};
}