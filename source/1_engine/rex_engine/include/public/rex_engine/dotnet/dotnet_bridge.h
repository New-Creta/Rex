#pragma once

#include "rex_std/string_view.h"

#include "rex_engine/engine/globals.h"

#include "rex_hostfxr/hostfxr.h"
#include "rex_nethost/coreclr_delegates.h"

#include "rex_engine/system/library_loader.h"

namespace rex
{
	// The dotnet bridge is essentially a wrapper around the Hostfxr library
	// It handles initialization of the library
	// and allows users to call into it to return functions from a dotnet library
	class DotNetBridge
	{
	public:
		DotNetBridge(rsl::string_view runtimeConfigPath);
		DotNetBridge(const DotNetBridge&) = delete;
		DotNetBridge(DotNetBridge&&) = delete;

		~DotNetBridge();

		DotNetBridge& operator=(const DotNetBridge&) = delete;
		DotNetBridge& operator=(DotNetBridge&&) = delete;

		// Load a function from a dotnet library using a class path and a method of that class
		void* load_function(rsl::string_view assemblyPath, rsl::string_view classPath, rsl::string_view methodName) const;

	private:

		// Init hostfxr itself, finding its path and loading the module
		void init_hostfxr();
		// Calls into the hostfxr module to load the api functions
		void init_hostfxr_funcs();
		// Loads clr api functions
		void init_clr_funcs(rsl::string_view runtimeConfigPath);

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
		load_assembly_and_get_function_pointer_fn m_load_assembly_and_get_function_pointer_fn;
	};

	namespace dotnet
	{
		void init(globals::GlobalUniquePtr<DotNetBridge> hostFxrLib);
		DotNetBridge* instance();
		void shutdown();
	}
}