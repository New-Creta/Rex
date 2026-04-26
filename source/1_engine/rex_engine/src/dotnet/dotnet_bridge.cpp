#include "rex_engine/dotnet/dotnet_bridge.h"

#include "rex_engine/diagnostics/assert.h"

#include "rex_nethost/nethost.h"
#include "rex_engine/filesystem/path.h"

#include "rex_engine/text_processing/text_processing.h"

namespace rex
{
	DEFINE_LOG_CATEGORY(LogHostFxr);

	DotNetBridge::DotNetBridge(rsl::string_view runtimeConfigPath)
		: m_module(nullptr)
		, m_init_for_cmd_line_fptr(nullptr)
		, m_init_for_config_fptr(nullptr)
		, m_get_delegate_fptr(nullptr)
		, m_run_app_fptr(nullptr)
		, m_close_fptr(nullptr)
		, m_config_handle(nullptr)
		, m_load_assembly_and_get_function_pointer_fn(nullptr)
	{
		init_hostfxr();
		init_hostfxr_funcs();
		init_clr_funcs(runtimeConfigPath);
	}

	DotNetBridge::~DotNetBridge()
	{
		m_close_fptr(m_config_handle);
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

	// Init hostfxr itself, finding its path and loading the module
	void DotNetBridge::init_hostfxr()
	{
		get_hostfxr_parameters params{ sizeof(get_hostfxr_parameters), nullptr, nullptr };
		char_t hostfxr_path[rex::path::max_path_length()];
		size_t hostfxr_path_length = sizeof(hostfxr_path) / sizeof(char_t);

		REX_ASSERT_X(get_hostfxr_path(hostfxr_path, &hostfxr_path_length, &params) == 0, "Failed to find hostfxr path");
		temp_string hostfxr_path_ascii = rex::to_string<temp_string>(hostfxr_path);
		m_module = load_library(hostfxr_path_ascii);
		REX_ASSERT_X(m_module != nullptr, "Failed to load hostfxr module");
	}
	// Calls into the hostfxr module to load the api functions
	void DotNetBridge::init_hostfxr_funcs()
	{
		m_init_for_cmd_line_fptr = (hostfxr_initialize_for_dotnet_command_line_fn)load_procedure(m_module, "hostfxr_initialize_for_dotnet_command_line");
		m_init_for_config_fptr = (hostfxr_initialize_for_runtime_config_fn)load_procedure(m_module, "hostfxr_initialize_for_runtime_config");
		m_get_delegate_fptr = (hostfxr_get_runtime_delegate_fn)load_procedure(m_module, "hostfxr_get_runtime_delegate");
		m_run_app_fptr = (hostfxr_run_app_fn)load_procedure(m_module, "hostfxr_run_app");
		m_close_fptr = (hostfxr_close_fn)load_procedure(m_module, "hostfxr_close");
	}
	// Loads clr api functions
	void DotNetBridge::init_clr_funcs(rsl::string_view runtimeConfigPath)
	{
		// Load .NET Core
		temp_wstring wide_runtime_config_path = rex::to_wstring<temp_wstring>(runtimeConfigPath);
		int rc = m_init_for_config_fptr(wide_runtime_config_path.data(), nullptr, &m_config_handle);
		if (rc != 0 || m_config_handle == nullptr)
		{
			REX_ERROR(LogHostFxr, "Failed to initialize clr functions of host fxr lib. Error: {}", rc);
			m_close_fptr(m_config_handle);
			return;
		}

		// Get the load assembly function pointer
		void* load_assembly_and_get_function_pointer = nullptr;
		rc = m_get_delegate_fptr(
			m_config_handle,
			hdt_load_assembly_and_get_function_pointer,
			&load_assembly_and_get_function_pointer);
		if (rc != 0 || load_assembly_and_get_function_pointer == nullptr)
		{
			REX_ERROR(LogHostFxr, "Get delegate failed: }", rc);
		}

		m_load_assembly_and_get_function_pointer_fn = (load_assembly_and_get_function_pointer_fn)load_assembly_and_get_function_pointer;
	}

	namespace dotnet
	{
		globals::GlobalUniquePtr<DotNetBridge> g_host_fxr_lib;
		void init(globals::GlobalUniquePtr<DotNetBridge> hostFxrLib)
		{
			g_host_fxr_lib = rsl::move(hostFxrLib);
		}
		DotNetBridge* instance()
		{
			return g_host_fxr_lib.get();
		}
		void shutdown()
		{
			g_host_fxr_lib.reset();
		}
	}
}