#include "rex_directx/directx_feature_level.h"
#include "rex_directx/directx_util.h"
#include "rex_directx/dxgi/util.h"

#include "rex_std/array.h"

namespace rex
{
    const rsl::array EXPECTED_FEATURE_LEVELS =
    {
        D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_1
    };

    //-------------------------------------------------------------------------
    rsl::string feature_level_name(const D3D_FEATURE_LEVEL& level)
    {
        switch(level)
        {
            case D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_0: return rsl::string("D3D_FEATURE_LEVEL_12_0");
            case D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_1: return rsl::string("D3D_FEATURE_LEVEL_12_1");
            case D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_1_0_CORE: return rsl::string("D3D_FEATURE_LEVEL_1_0_CORE");
        }

        //assert(false, "Unknown feature level");
        return {};
    }

    //-------------------------------------------------------------------------
    bool is_correct_feature_level(s32 level)
    {
        return rsl::cend(EXPECTED_FEATURE_LEVELS) != rsl::find(rsl::cbegin(EXPECTED_FEATURE_LEVELS), rsl::cend(EXPECTED_FEATURE_LEVELS), level);
    }

    //-------------------------------------------------------------------------
    FeatureLevelInfo query_feature_level(IDXGIAdapter* adapter)
    {
        // backwards looping as it's checking for a minimum feature level
        for (auto it = EXPECTED_FEATURE_LEVELS.crbegin(); it != EXPECTED_FEATURE_LEVELS.crend(); ++it)
        {
            D3D_FEATURE_LEVEL feature_level = *it;
            if (SUCCEEDED(D3D12CreateDevice(adapter, feature_level, __uuidof(ID3D12Device), nullptr)))
            {
                return { static_cast<s32>(feature_level), feature_level_name(feature_level) };
            }
        }

        //assert( "At least D3D_FEATURE_LEVEL_12_0 has to be supported for DirectX 12!");

        // If the compiler doesn't recognise D3D_FEATURE_LEVEL_1_0_CORE
        // Make sure you're using windows SDK 10.0.18362.0 or later
        return { static_cast<s32>(D3D_FEATURE_LEVEL_1_0_CORE), feature_level_name(D3D_FEATURE_LEVEL_1_0_CORE) };
    }
}