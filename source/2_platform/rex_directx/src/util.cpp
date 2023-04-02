#include "dx12/core/dx12util.h"

//-------------------------------------------------------------------------
bool rex::dx12::isCorrectFeatureLevel(D3D_FEATURE_LEVEL level)
{
	return std::cend(rex::dx12::EXPECTED_FEATURE_LEVELS) != std::find(std::cbegin(rex::dx12::EXPECTED_FEATURE_LEVELS), std::cend(rex::dx12::EXPECTED_FEATURE_LEVELS), level);
}

//-------------------------------------------------------------------------
D3D_FEATURE_LEVEL rex::dx12::queryFeatureLevel(IDXGIAdapter* adapter)
{
	// backwards looping as it's checking for a minimum feature level
	for (auto it = rex::dx12::EXPECTED_FEATURE_LEVELS.crbegin(); it != rex::dx12::EXPECTED_FEATURE_LEVELS.crend(); ++it)
	{
		D3D_FEATURE_LEVEL feature_level = *it;
		if (SUCCEEDED(D3D12CreateDevice(adapter, feature_level, __uuidof(ID3D12Device), nullptr)))
			return feature_level;
	}

	RX_ASSERT( "At least D3D_FEATURE_LEVEL_12_0 has to be supported for DirectX 12!");

	// If the compiler doesn't recognise D3D_FEATURE_LEVEL_1_0_CORE
	// Make sure you're using windows SDK 10.0.18362.0 or later
	return D3D_FEATURE_LEVEL_1_0_CORE;
}