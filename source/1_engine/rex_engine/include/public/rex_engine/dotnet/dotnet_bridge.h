#pragma once

#include "rex_std/string_view.h"

#include "rex_engine/engine/globals.h"

#include "rex_engine/dotnet/dotnet_runtime.h"
#include "rex_engine/system/library_loader.h"

#include "rex_nethost/coreclr_delegates.h"
#include "rex_hostfxr/hostfxr.h"

namespace rex
{
	// IMPORTANT NOTE
	// If you want to put breakpoints in C# code (using Visual Studio)
	// Go to the project settings of your VS C++ project > Properties > Debugging > Debugger Type = Mixed
	// And make sure "Just my code" is disabled for both C++ and C# debugging
	// See: https://stackoverflow.com/questions/27169558/skipped-loading-symbols-for-ngen-binary-for-c-sharp-dll

	// The dotnet bridge allows the user to set delegate defined in C# to point back into C++
	// It's not possible to load C# functions into C++ callback variables (only internally)
	// as that doesn't matches the engine's design.
	// The engine is only allowed to call into C# to run its internal ticks
	// The user should not call into C# API to perform certain functionality
	// if such functionality is desired, the functionality should be moved to C++ instead
	class DotNetBridge
	{
	public:
		DotNetBridge(rsl::string_view runtimeConfigPath);
		DotNetBridge(const DotNetBridge&) = delete;
		DotNetBridge(DotNetBridge&&) = delete;
		DotNetBridge& operator=(const DotNetBridge&) = delete;
		DotNetBridge& operator=(DotNetBridge&&) = delete;

		// Set a function in C# to point back into C++ code
		void set_delegate(rsl::string_view classPath, rsl::string_view delegateName, void* fn);
		// Load a function from a dotnet library using a class path and a method of that class
		void* load_function(rsl::string_view assemblyPath, rsl::string_view classPath, rsl::string_view methodName) const;
		template <typename Func>
		Func load_function(rsl::string_view assemblyPath, rsl::string_view classPath, rsl::string_view methodName) const
		{
			return (Func)load_function(assemblyPath, classPath, methodName);
		}

	private:
		load_assembly_and_get_function_pointer_fn m_load_assembly_and_get_function_pointer_fn;
		DotNetRuntime m_dotnet_runtime;
		rsl::string m_dotnet_bridge_assembly_path;

		using set_delegate_fn = void(*)(const char*, const char*, void*);
		set_delegate_fn m_set_delegate_fn;
	};
}