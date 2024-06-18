#include "rex_directx/system/dx_shader_reflection.h"

#include "rex_directx/diagnostics/dx_call.h"

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

    DxShaderReflection::DxShaderReflection(rsl::string_view shaderCode)
    {
      DX_CALL(D3DReflect(shaderCode.data(), shaderCode.length(), IID_PPV_ARGS(m_reflection_object.GetAddressOf())));
      D3D12_SHADER_DESC shader_desc;
      DX_CALL(m_reflection_object->GetDesc(&shader_desc));

      s32 num_constant_buffers  = shader_desc.ConstantBuffers;
      s32 num_input_params      = shader_desc.InputParameters;
      s32 num_output_params     = shader_desc.OutputParameters;
      s32 num_bound_resources   = shader_desc.BoundResources;

      reflect_constant_buffers(num_constant_buffers);
      reflect_input_params(num_input_params);
      reflect_output_params(num_output_params);
      reflect_bound_resources(num_bound_resources);
    }
  }
}