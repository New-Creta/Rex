#ifndef DX12UTIL
#define DX12UTIL

#pragma once

//////////////////////////////////////////////////////////////////////////
//
//
// If you are not able to include one of these files it might be due to the following reasons
// 
// - Your OS version is lower than Windows 10
//          * To be able to run Rex at all you'll have to upgrade to Windows 10.
// - You did not install the latest version of the Windows 10 SDK
//          * Download the latest version from this link: https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk
//          * Try compiling again after installation
// - Check your visual studio version
//          * To build Rex you will need at least Visual Studio 2019
//          * Check if you have any updates to apply on your version of Visual Studio
//          * Try compiling again after installation
// 
//  If you still have issues compiling Rex you can contact us using the following mail addresses:
//
//  davedebreuck.gamedeveloper@gmail.com
//  nickdebreuck.gamedeveloper@gmail.com
//
//  
//////////////////////////////////////////////////////////////////////////

#include <d3d12.h>
#include <d3d11on12.h>

#pragma comment(lib, "d3d12.lib")

#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>

#include "d3dx12.h"
#include "rex_graphics_global.h"

#include <array>

namespace rex
{
    namespace dx12
    {
        static const std::array EXPECTED_FEATURE_LEVELS =
        {
            D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_0,
            D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_1
        };

        REX_GRAPHICS_EXPORT bool isCorrectFeatureLevel(D3D_FEATURE_LEVEL level);

		REX_GRAPHICS_EXPORT D3D_FEATURE_LEVEL queryFeatureLevel(IDXGIAdapter* adapter);
    }
}

#endif