#pragma once

#include "rex_directx/utility/dx_util.h"
#include "rex_std/string_view.h"
#include <d3d12shader.h>

namespace rex
{
  namespace gfx
  {
    class DxShaderReflection
    {
    public:
      DxShaderReflection(rsl::string_view sourceCode);

    private:
      void reflect_constant_buffers(s32 numConstantBuffers);
      void reflect_input_params(s32 numInputParams);
      void reflect_output_params(s32 numOutputParams);
      void reflect_bound_resources(s32 numBoundResources);

    private:
      wrl::ComPtr<ID3D12ShaderReflection> m_reflection_object;

      rsl::vector<ConstantBufferReflection> m_cb_reflection;
      rsl::vector<ShaderParameterReflection> m_input_params;
      rsl::vector<ShaderParameterReflection> m_output_params;
      rsl::vector<BoundResourceReflection> m_samplers;
      rsl::vector<BoundResourceReflection> m_textures;
    };
  }
}