#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/system/system_info.h"

TEST_CASE("TEST - System - SystemInfo")
{
	rex::SystemInfo sys_info = rex::system_info();

#ifdef REX_PLATFORM_WINDOWS
	REX_CHECK(sys_info.active_platform() == rex::PlatformType::Windows);
#else
#error Invalid Platform
#endif

#ifdef REX_API_DIRECTX12
	REX_CHECK(sys_info.active_graphics_api() == rex::GraphicsAPIType::DirectX12);
#else
#error Invalid Graphics API
#endif

#ifdef REX_PLATFORM_X64
	REX_CHECK(sys_info.active_architecture() == rex::ArchitectureType::Bit64);
#else
#error Invalid Architecture
#endif
}