#include "rex_platform_pch.h"
#include "rex_platform_defines.h"

#include "core/win32application.h"

#include <VersionHelpers.h>

//-------------------------------------------------------------------------
rex::win32::Application::Application()
{
    RX_INFO("Platform: {}", REX_BUILD_TARGET);
    RX_INFO("Platform Build Version: {}", REX_BUILD_TARGET_VERSION);
    RX_INFO("Architecture: {}", REX_ARCHITECTURE);
}
//-------------------------------------------------------------------------
rex::win32::Application::~Application() = default;

//-------------------------------------------------------------------------
void rex::win32::Application::platformInitialize()
{
    RX_ASSERT_X(IsWindows10OrGreater() && !IsWindowsServer(), "We need at least a version of Windows 10 to run REX");

    appInitialize();
}

//-------------------------------------------------------------------------
void rex::win32::Application::platformUpdate(float dTime)
{
    appUpdate(dTime);
}

//-------------------------------------------------------------------------
void rex::win32::Application::platformQuit()
{
    appQuit();
}