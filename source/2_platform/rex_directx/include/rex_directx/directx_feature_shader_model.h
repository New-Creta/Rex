#pragma once

#include "rex_engine/types.h"
#include "rex_directx/directx_util.h"

#include "rex_std/string.h"

struct ID3D12Device;

namespace rex
{
    struct ShaderModelInfo
    {
        s32         version;
        rsl::string name;
    };

    //-------------------------------------------------------------------------
    rsl::string shader_model_name(const D3D_SHADER_MODEL& shaderModel)
    {
        switch(shaderModel)
        {
            case D3D_SHADER_MODEL::D3D_SHADER_MODEL_6_6: return rsl::string("D3D_SHADER_MODEL_6_6");
            case D3D_SHADER_MODEL::D3D_SHADER_MODEL_6_5: return rsl::string("D3D_SHADER_MODEL_6_5");
            case D3D_SHADER_MODEL::D3D_SHADER_MODEL_6_4: return rsl::string("D3D_SHADER_MODEL_6_4");
            case D3D_SHADER_MODEL::D3D_SHADER_MODEL_6_2: return rsl::string("D3D_SHADER_MODEL_6_2");
            case D3D_SHADER_MODEL::D3D_SHADER_MODEL_6_1: return rsl::string("D3D_SHADER_MODEL_6_1");
            case D3D_SHADER_MODEL::D3D_SHADER_MODEL_6_0: return rsl::string("D3D_SHADER_MODEL_6_0");
            case D3D_SHADER_MODEL::D3D_SHADER_MODEL_5_1: return rsl::string("D3D_SHADER_MODEL_5_1");
        }

        //assert(false, "Unknown shader model");
        return {};
    }

    //-------------------------------------------------------------------------
    bool check_for_shader_model_support(ID3D12Device* device, s32 version)
    {
        D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = {};
        shaderModel.HighestShaderModel = (D3D_SHADER_MODEL)version;

        HRESULT hr = device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel));
        if(FAILED(hr))
        {
            return false;
        }

        return true;
    }
    
    //-------------------------------------------------------------------------
    ShaderModelInfo query_shader_model_version(ID3D12Device* device)
    {
        D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = {};

        #if defined(NTDDI_WIN10_VB) && (NTDDI_VERSION >= NTDDI_WIN10_VB)
            shaderModel.HighestShaderModel = D3D_SHADER_MODEL_6_6;
        #elif defined(NTDDI_WIN10_19H1) && (NTDDI_VERSION >= NTDDI_WIN10_19H1)
            shaderModel.HighestShaderModel = D3D_SHADER_MODEL_6_5;
        #elif defined(NTDDI_WIN10_RS5) && (NTDDI_VERSION >= NTDDI_WIN10_RS5)
            shaderModel.HighestShaderModel = D3D_SHADER_MODEL_6_4;
        #elif defined(NTDDI_WIN10_RS4) && (NTDDI_VERSION >= NTDDI_WIN10_RS4)
            shaderModel.HighestShaderModel = D3D_SHADER_MODEL_6_2;
        #elif defined(NTDDI_WIN10_RS3) && (NTDDI_VERSION >= NTDDI_WIN10_RS3)
            shaderModel.HighestShaderModel = D3D_SHADER_MODEL_6_1;
        #else
            shaderModel.HighestShaderModel = D3D_SHADER_MODEL_6_0;
        #endif

        HRESULT hr = device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel));
        if(SUCCEEDED(hr))
        {
            return { (s32)shaderModel.HighestShaderModel, shader_model_name(shaderModel.HighestShaderModel)};
        }

        while (hr == E_INVALIDARG && shaderModel.HighestShaderModel > D3D_SHADER_MODEL_6_0)
        {
            shaderModel.HighestShaderModel = static_cast<D3D_SHADER_MODEL>(static_cast<int>(shaderModel.HighestShaderModel) - 1);
            hr = device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel));
            if(SUCCEEDED(hr))
            {
                return { (s32)shaderModel.HighestShaderModel, shader_model_name(shaderModel.HighestShaderModel)};
            }
        }

        if (FAILED(hr))
        {
            shaderModel.HighestShaderModel = D3D_SHADER_MODEL_5_1;
        }

        return { (s32)shaderModel.HighestShaderModel, shader_model_name(shaderModel.HighestShaderModel)};
    }
}