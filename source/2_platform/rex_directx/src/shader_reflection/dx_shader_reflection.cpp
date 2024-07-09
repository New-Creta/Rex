#include "rex_directx/shader_reflection/dx_shader_reflection.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_directx/system/dx_shader.h"

#include <d3dcompiler.h>

namespace rex
{
  namespace gfx
  {
    using ID3DShaderReflection = ID3D12ShaderReflection;
    using ID3DShaderReflectionVariable = ID3D12ShaderReflectionVariable;
    using ID3DShaderReflectionConstantBuffer = ID3D12ShaderReflectionConstantBuffer;
    using ID3DShaderReflectionType = ID3D12ShaderReflectionType;
    using D3D_SHADER_DESC = D3D12_SHADER_DESC;
    using D3D_SHADER_VARIABLE_DESC = D3D12_SHADER_VARIABLE_DESC;
    using D3D_SHADER_TYPE_DESC = D3D12_SHADER_TYPE_DESC;
    using D3D_SHADER_BUFFER_DESC = D3D12_SHADER_BUFFER_DESC;
    using D3D_SHADER_INPUT_BIND_DESC = D3D12_SHADER_INPUT_BIND_DESC;
    using D3D_SIGNATURE_PARAMETER_DESC = D3D12_SIGNATURE_PARAMETER_DESC;

    DxShaderReflection::DxShaderReflection(const wrl::ComPtr<ID3DBlob>& shaderCompiledCode, ShaderType type)
      : ShaderSignature(type)
    {
      DX_CALL(D3DReflect(shaderCompiledCode->GetBufferPointer(), shaderCompiledCode->GetBufferSize(), IID_PPV_ARGS(m_reflection_object.GetAddressOf())));
      D3D12_SHADER_DESC shader_desc;
      DX_CALL(m_reflection_object->GetDesc(&shader_desc));

      m_shader_version = convert_version_to_string(shader_desc.Version);

      s32 num_constant_buffers  = shader_desc.ConstantBuffers;
      s32 num_input_params      = shader_desc.InputParameters;
      s32 num_output_params     = shader_desc.OutputParameters;
      s32 num_bound_resources   = shader_desc.BoundResources;

      reflect_constant_buffers(num_constant_buffers);
      reflect_input_params(num_input_params);
      reflect_output_params(num_output_params);
      reflect_bound_resources(num_bound_resources);
    }

    void DxShaderReflection::reflect_constant_buffers(s32 numConstantBuffers)
    {
      rsl::vector<ConstantBufferReflection> constant_buffers;
      constant_buffers.reserve(numConstantBuffers);

      for (card32 i = 0; i < numConstantBuffers; ++i)
      {
        ID3D12ShaderReflectionConstantBuffer* cb = m_reflection_object->GetConstantBufferByIndex(i);
        //constant_buffers.emplace_back(cb, get_constant_buffer_register(m_reflection_object.Get(), cb));
      }

      init_constant_buffers(rsl::move(constant_buffers));
    }
    void DxShaderReflection::reflect_input_params(s32 numInputParams)
    {
      rsl::vector<ShaderParamReflection> input_params;
      input_params.reserve(numInputParams);

      for (card32 i = 0; i < numInputParams; ++i)
      {
        D3D12_SIGNATURE_PARAMETER_DESC param_desc;
        DX_CALL(m_reflection_object->GetInputParameterDesc(i, &param_desc));
        //input_params.emplace_back(param_desc);
      }

      init_input_params(rsl::move(input_params));
    }
    void DxShaderReflection::reflect_output_params(s32 numOutputParams)
    {
      rsl::vector<ShaderParamReflection> output_params;
      output_params.reserve(numOutputParams);

      for (card32 i = 0; i < numOutputParams; ++i)
      {
        D3D12_SIGNATURE_PARAMETER_DESC param_desc;
        DX_CALL(m_reflection_object->GetOutputParameterDesc(i, &param_desc));
        //output_params.emplace_back(param_desc);
      }

      init_output_params(rsl::move(output_params));
    }
    void DxShaderReflection::reflect_bound_resources(s32 numBoundResources)
    {
      rsl::vector<BoundResourceReflection> textures;
      rsl::vector<BoundResourceReflection> samplers;

      for (card32 i = 0; i < numBoundResources; ++i)
      {
        D3D12_SHADER_INPUT_BIND_DESC resource_desc;
        DX_CALL(m_reflection_object->GetResourceBindingDesc(i, &resource_desc));

        switch (resource_desc.Type)
        {
        case D3D_SIT_CBUFFER: continue;
        case D3D_SIT_TEXTURE: /*textures.emplace_back(resource_desc);*/ break;
        case D3D_SIT_SAMPLER: /*samplers.emplace_back(resource_desc);*/ break;
        default: REX_ASSERT("Invalid bound resource type");
        }

        init_textures(rsl::move(textures));
        init_samplers(rsl::move(samplers));
      }
    }
    card32 DxShaderReflection::get_constant_buffer_register(ID3D12ShaderReflection* refl, ID3D12ShaderReflectionConstantBuffer* cb_refl)
    {
      D3D12_SHADER_BUFFER_DESC cb_desc;
      DX_CALL(cb_refl->GetDesc(&cb_desc));

      D3D12_SHADER_INPUT_BIND_DESC input_desc;
      DX_CALL(refl->GetResourceBindingDescByName(cb_desc.Name, &input_desc));

      return input_desc.BindPoint;
    }
    rsl::tiny_stack_string DxShaderReflection::convert_version_to_string(UINT version)
    {
      D3D12_SHADER_VERSION_TYPE shader_type = static_cast<D3D12_SHADER_VERSION_TYPE>((version & 0xFFFF0000) >> 16);
      s32 major = (version & 0x000000F0) >> 4;
      s32 minor = (version & 0x0000000F);

      rsl::tiny_stack_string version_str;
      switch (shader_type)
      {
      case D3D12_SHVER_PIXEL_SHADER:     version_str = "ps_"; break;
      case D3D12_SHVER_VERTEX_SHADER:    version_str = "vs_"; break;
      case D3D12_SHVER_GEOMETRY_SHADER:  version_str = "gs_"; break;
      case D3D12_SHVER_HULL_SHADER:      version_str = "hs_"; break;
      case D3D12_SHVER_DOMAIN_SHADER:    version_str = "ds_"; break;
      case D3D12_SHVER_COMPUTE_SHADER:   version_str = "cs_"; break;
      }

      version_str += rsl::format("{}_{}", major, minor);

      return version_str;
    }

		namespace rhi
		{
			rsl::unique_ptr<gfx::ShaderSignature> reflect_shader(const gfx::Shader* shader)
			{
				const gfx::DxShader* dx_shader = static_cast<const gfx::DxShader*>(shader);

				return rsl::make_unique<DxShaderReflection>(dx_shader->dx_bytecode(), dx_shader->type());
			}
		}
  }
}