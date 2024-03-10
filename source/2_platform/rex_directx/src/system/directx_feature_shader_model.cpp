#include "rex_directx/system/directx_feature_shader_model.h"

#include <Windows.h>
#include <sdkddkver.h>

namespace rex
{
  //-------------------------------------------------------------------------
  rsl::string_view shader_model_name(D3D_SHADER_MODEL shaderModel)
  {
    switch(shaderModel)
    {
      case D3D_SHADER_MODEL::D3D_SHADER_MODEL_6_6: return rsl::string_view("D3D_SHADER_MODEL_6_6");
      case D3D_SHADER_MODEL::D3D_SHADER_MODEL_6_5: return rsl::string_view("D3D_SHADER_MODEL_6_5");
      case D3D_SHADER_MODEL::D3D_SHADER_MODEL_6_4: return rsl::string_view("D3D_SHADER_MODEL_6_4");
      case D3D_SHADER_MODEL::D3D_SHADER_MODEL_6_2: return rsl::string_view("D3D_SHADER_MODEL_6_2");
      case D3D_SHADER_MODEL::D3D_SHADER_MODEL_6_1: return rsl::string_view("D3D_SHADER_MODEL_6_1");
      case D3D_SHADER_MODEL::D3D_SHADER_MODEL_6_0: return rsl::string_view("D3D_SHADER_MODEL_6_0");
      case D3D_SHADER_MODEL::D3D_SHADER_MODEL_5_1: return rsl::string_view("D3D_SHADER_MODEL_5_1");
      default: return rsl::string_view("Unknown shader model");
    }
  }

  //-------------------------------------------------------------------------
  bool check_for_shader_model_support(ID3D12Device* device, D3D_SHADER_MODEL version)
  {
    D3D12_FEATURE_DATA_SHADER_MODEL shader_model = {};
    shader_model.HighestShaderModel              = version;

    const HRESULT hr = device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shader_model, sizeof(shader_model));

    return SUCCEEDED(hr);
  }

  //-------------------------------------------------------------------------
  D3D_SHADER_MODEL query_shader_model_version(ID3D12Device* device)
  {
    D3D12_FEATURE_DATA_SHADER_MODEL shader_model = {};

#if defined(NTDDI_WIN10_VB) && (NTDDI_VERSION >= NTDDI_WIN10_VB)
    shader_model.HighestShaderModel = D3D_SHADER_MODEL_6_6;
#elif defined(NTDDI_WIN10_19H1) && (NTDDI_VERSION >= NTDDI_WIN10_19H1)
    shader_model.HighestShaderModel = D3D_SHADER_MODEL_6_5;
#elif defined(NTDDI_WIN10_RS5) && (NTDDI_VERSION >= NTDDI_WIN10_RS5)
    shader_model.HighestShaderModel = D3D_SHADER_MODEL_6_4;
#elif defined(NTDDI_WIN10_RS4) && (NTDDI_VERSION >= NTDDI_WIN10_RS4)
    shader_model.HighestShaderModel = D3D_SHADER_MODEL_6_2;
#elif defined(NTDDI_WIN10_RS3) && (NTDDI_VERSION >= NTDDI_WIN10_RS3)
    shader_model.HighestShaderModel = D3D_SHADER_MODEL_6_1;
#else
    shader_model.HighestShaderModel = D3D_SHADER_MODEL_6_0;
#endif

    HRESULT hr = device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shader_model, sizeof(shader_model));
    if(SUCCEEDED(hr))
    {
      return shader_model.HighestShaderModel;
    }

    while(hr == E_INVALIDARG && shader_model.HighestShaderModel > D3D_SHADER_MODEL_6_0)
    {
      shader_model.HighestShaderModel = static_cast<D3D_SHADER_MODEL>(static_cast<int>(shader_model.HighestShaderModel) - 1);
      hr                              = device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shader_model, sizeof(shader_model));
      if(SUCCEEDED(hr))
      {
        return shader_model.HighestShaderModel;
      }
    }

    if(FAILED(hr))
    {
      shader_model.HighestShaderModel = D3D_SHADER_MODEL_5_1;
    }

    return shader_model.HighestShaderModel;
  }
} // namespace rex