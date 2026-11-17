#include "rex_engine/dotnet/dotnet_runtime.h"

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/text_processing/text_processing.h"

#include "rex_nethost/nethost.h"

namespace rex
{
	DEFINE_LOG_CATEGORY(LogDotNetRuntime);

	DotNetRuntime::DotNetRuntime()
		: m_module(nullptr)
		, m_init_for_cmd_line_fptr(nullptr)
		, m_init_for_config_fptr(nullptr)
		, m_get_delegate_fptr(nullptr)
		, m_run_app_fptr(nullptr)
		, m_close_fptr(nullptr)
		, m_config_handle(nullptr)
	{
		init_hostfxr();
		init_hostfxr_funcs();
	}

	DotNetRuntime::~DotNetRuntime()
	{
		m_close_fptr(m_config_handle);
	}

	// Init hostfxr itself, finding its path and loading the module
	void DotNetRuntime::init_hostfxr()
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
	void DotNetRuntime::init_hostfxr_funcs()
	{
		m_init_for_cmd_line_fptr = (hostfxr_initialize_for_dotnet_command_line_fn)load_procedure(m_module, "hostfxr_initialize_for_dotnet_command_line");
		m_init_for_config_fptr = (hostfxr_initialize_for_runtime_config_fn)load_procedure(m_module, "hostfxr_initialize_for_runtime_config");
		m_get_delegate_fptr = (hostfxr_get_runtime_delegate_fn)load_procedure(m_module, "hostfxr_get_runtime_delegate");
		m_run_app_fptr = (hostfxr_run_app_fn)load_procedure(m_module, "hostfxr_run_app");
		m_close_fptr = (hostfxr_close_fn)load_procedure(m_module, "hostfxr_close");
	}
	// Loads clr api functions
	load_assembly_and_get_function_pointer_fn DotNetRuntime::load_get_function_delegate(rsl::string_view runtimeConfigPath)
	{
		// Load .NET Core
		temp_wstring wide_runtime_config_path = rex::to_wstring<temp_wstring>(runtimeConfigPath);
		int rc = m_init_for_config_fptr(wide_runtime_config_path.data(), nullptr, &m_config_handle);
		if (rc != 0 || m_config_handle == nullptr)
		{
			REX_ERROR(LogDotNetRuntime, "Failed to initialize clr functions of host fxr lib. Error: {}", rc);
			m_close_fptr(m_config_handle);
			return nullptr;
		}

		// Get the load assembly function pointer
		void* load_assembly_and_get_function_pointer = nullptr;
		rc = m_get_delegate_fptr(
			m_config_handle,
			hdt_load_assembly_and_get_function_pointer,
			&load_assembly_and_get_function_pointer);
		if (rc != 0 || load_assembly_and_get_function_pointer == nullptr)
		{
			REX_ERROR(LogDotNetRuntime, "Get delegate failed: }", rc);
		}

		return (load_assembly_and_get_function_pointer_fn)load_assembly_and_get_function_pointer;
	}
}