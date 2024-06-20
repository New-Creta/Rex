#pragma once

#include "rex_directx/utility/dx_util.h"
#include "rex_std/string_view.h"
#include "rex_std/bonus/string.h"
#include <d3d12shader.h>

#include "rex_directx/shader_reflection/dx_bound_resource_reflection.h"
#include "rex_directx/shader_reflection/dx_constant_buffer_reflection.h"
#include "rex_directx/shader_reflection/dx_constant_buffer_variable_reflection.h"
#include "rex_directx/shader_reflection/dx_shader_param_reflection.h"

#include "rex_renderer_core/shader_reflection/shader_reflection.h"

namespace rex
{
  namespace gfx
  {
    class DxShaderReflection : public ShaderSignature
    {
    public:
      DxShaderReflection(const wrl::ComPtr<ID3DBlob>& shaderCompiledCode, ShaderType type);

    private:
      void reflect_constant_buffers(s32 numConstantBuffers);
      void reflect_input_params(s32 numInputParams);
      void reflect_output_params(s32 numOutputParams);
      void reflect_bound_resources(s32 numBoundResources);
      s32 get_constant_buffer_register(ID3D12ShaderReflection* refl, ID3D12ShaderReflectionConstantBuffer* cb_refl);
      rsl::tiny_stack_string convert_version_to_string(UINT version);

    private:
      wrl::ComPtr<ID3D12ShaderReflection> m_reflection_object;

      rsl::tiny_stack_string m_shader_version;
    };
  }
}