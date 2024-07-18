#pragma once

#include "rex_directx/utility/dx_util.h"
#include "rex_std/string_view.h"
#include "rex_std/bonus/string.h"
#include <d3d12shader.h>

#include "rex_directx/shader_reflection/dx_bound_resource_reflection.h"
#include "rex_directx/shader_reflection/dx_constant_buffer_reflection.h"
#include "rex_directx/shader_reflection/dx_constant_buffer_variable_reflection.h"
#include "rex_directx/shader_reflection/dx_shader_param_reflection.h"

#include "rex_renderer_core/shader_reflection/shader_signature.h"

namespace rex
{
  namespace gfx
  {
    class DxShaderReflection : public ShaderSignature
    {
    public:
      DxShaderReflection(const wrl::ComPtr<ID3DBlob>& shaderCompiledCode, ShaderType type);
      DxShaderReflection(const D3D12_SHADER_BYTECODE& shaderCompiledCode, ShaderType type);

    private:
      void reflect_constant_buffers(s32 numConstantBuffers);
      void reflect_input_params(s32 numInputParams);
      void reflect_output_params(s32 numOutputParams);
      void reflect_bound_resources(s32 numBoundResources);
      s32 get_constant_buffer_register(ID3D12ShaderReflection* refl, ID3D12ShaderReflectionConstantBuffer* cb_refl);
      rsl::tiny_stack_string convert_version_to_string(UINT version);

    private:
      DxShaderReflection(const void* byteCode, s32 byteCount, ShaderType type);

      CBufferReflDesc reflect_constant_buffer(ID3D12ShaderReflectionConstantBuffer* cbReflection);
      ShaderParamReflection reflect_shader_input_parameter(s32 idx);
      ShaderParamReflection reflect_shader_output_parameter(s32 idx);
      BoundResourceReflection reflect_bound_resource(s32 idx);

    private:
      wrl::ComPtr<ID3D12ShaderReflection> m_reflection_object;

      rsl::tiny_stack_string m_shader_version;
    };

    ShaderParameterType shader_param_type(D3D_REGISTER_COMPONENT_TYPE componentType, s32 componentMask, D3D_MIN_PRECISION precision);
    ShaderParameterType component_mask_to_float(s32 componentMask);
    ShaderParameterType component_mask_to_uint(s32 componentMask, D3D_MIN_PRECISION precision);
  }
}