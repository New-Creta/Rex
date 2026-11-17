#include "rex_engine/dotnet/dotnet_bridge.h"

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/engine/module_manager.h"

#include "rex_nethost/nethost.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"

#include "rex_engine/text_processing/text_processing.h"

namespace rex
{
	DEFINE_LOG_CATEGORY(LogHostFxr);

	DotNetBridge::DotNetBridge(rsl::string_view runtimeConfigPath)
		: m_dotnet_runtime()
		, m_load_assembly_and_get_function_pointer_fn()
		, m_set_delegate_fn()
	{
		m_load_assembly_and_get_function_pointer_fn = m_dotnet_runtime.load_get_function_delegate(runtimeConfigPath);
		const Module* engine_csharp_module = module_manager::instance()->current()->find_runtime_dependency("RexEngineCSharp");
		REX_ASSERT_X(engine_csharp_module, "Engine's C# cannot be found");

		// The C# module is the glue between C++ and C#. 
		// We load the function that can set delegates in C# pointing back to C++ here
		m_dotnet_bridge_assembly_path.assign(engine_csharp_module->target_path());
		REX_ASSERT_X(file::exists(m_dotnet_bridge_assembly_path), "Filepath to RexengineCSharp dll does not exist. '{}'", m_dotnet_bridge_assembly_path);
		m_set_delegate_fn = (set_delegate_fn)load_function(m_dotnet_bridge_assembly_path, "Rex.DelegateSetter", "SetDelegate");
	}

	void DotNetBridge::set_delegate(rsl::string_view classPath, rsl::string_view delegateName, void* fn)
	{ 
		m_set_delegate_fn(classPath.data(), delegateName.data(), fn);
	}

	// Load a function from a dotnet library using a class path and a method of that class
	void* DotNetBridge::load_function(rsl::string_view assemblyPath, rsl::string_view classPath, rsl::string_view methodName) const
	{
		temp_string type_name;
		type_name += classPath;
		type_name += ", ";
		type_name += assemblyPath.substr(assemblyPath.find_last_of("\\") + 1);
		type_name.assign(type_name.substr(0, type_name.find_last_of(".")));

		temp_wstring wide_type_name = rex::to_wstring<temp_wstring>(type_name);
		temp_wstring wide_assembly_path = rex::to_wstring<temp_wstring>(assemblyPath);
		temp_wstring wide_method_name = rex::to_wstring<temp_wstring>(methodName);

		// Format of the typename is
		// <namespace>.<classname>, <library filename without extension>
		void* func_ptr = nullptr;
		int rc = m_load_assembly_and_get_function_pointer_fn(
			wide_assembly_path.data(),							// The assembly filepath to be loaded ansd searched in
			wide_type_name.data(),									// The full "path" of the class. namespace and everything (eg. my.namespace.classname)
			wide_method_name.data(),								// The method/delegate in C# to bind the function to
			UNMANAGEDCALLERSONLY_METHOD,			// Specified to indicate we only care about unmanaged callers
			nullptr,													// Reserved, do not use (always set to nullptr)
			(void**)&func_ptr);								// The C++ function to bind to the C# delegate

		REX_ASSERT_X(rc == 0 && func_ptr != nullptr, "Failure: load_assembly_and_get_function_pointer()");

		return func_ptr;
	}
}